
#define SerialAT Serial2
#define SerialMon Serial
int RST_KEY = 15;
#define TINY_GSM_MODEM_A6
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false


// Your GPRS credentials, if any
const char apn[]      = "sl2sfr";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "Livebox-4296";
const char wifiPass[] = "Santony85";

// Server details
const char server[]   = "51.68.125.223";
const char resource[] = "/socket.io/?EIO=4";
const int  port       = 3333;

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);
HttpClient    http(client, server, port);

void setup() {
  // put your setup code here, to run once:
  pinMode(RST_KEY, OUTPUT);
  Serial.begin(115200);
  Serial.println("Start");
  SerialAT.begin(115200);
  digitalWrite(RST_KEY, LOW);
  delay(200);
  digitalWrite(RST_KEY, HIGH);
  delay(500);
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();
  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

}

void loop() {
  // put your main code here, to run repeatedly:
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");
  if (modem.isNetworkConnected()) { SerialMon.println("Network connected"); }
  
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }
  SerialMon.print(F("Performing HTTP GET request... "));
  int err = http.get(resource);
  if (err != 0) {
    SerialMon.println(F("failed to connect"));
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  SerialMon.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName  = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    SerialMon.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked()) {
    SerialMon.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

  SerialMon.print(F("Body length is: "));
  SerialMon.println(body.length());

  // Shutdown

  http.stop();
  SerialMon.println(F("Server disconnected"));
  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));


  // Do nothing forevermore
  while (true) { delay(1000); }

}
