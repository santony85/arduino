/**************************************************************
 * WebSocketClient SocketIO
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 *
 **************************************************************/

// Select your modem:
//#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900
 #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266

// Increase RX buffer if needed
//#define TINY_GSM_RX_BUFFER 512
int RST_KEY = 15;


#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

// Uncomment this if you want to see all AT commands
//#define DUMP_AT_COMMANDS

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial2

// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX


// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "sl2sfr";
const char user[] = "";
const char pass[] = "";

// MQTT details
const char* broker = "51.68.125.223";

const char* topicLed       = "GsmClientTest/led";
const char* topicInit      = "GsmClientTest/init";
const char* topicLedStatus = "GsmClientTest/ledStatus";

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <SocketIOclient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

PubSubClient  mqtt(client);
//SocketIOclient socketIO(client);

uint32_t lastReconnectAttempt = 0;
int ledStatus = LOW;

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();

  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    //digitalWrite(LED_PIN, ledStatus);
    mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker
  boolean status = mqtt.connect("GsmClientTest");

  // Or, if you want to authenticate MQTT:
  // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.publish(topicInit, "GsmClientTest started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}

void setup() {
  pinMode(RST_KEY, OUTPUT);
  SerialMon.begin(115200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(3000);
  digitalWrite(RST_KEY, LOW);
  delay(200);
  digitalWrite(RST_KEY, HIGH);
  delay(500);
  SerialMon.println(F("Initializing modem..."));
  modem.restart();
  //modem.init();
  String modemInfo = modem.getModemInfo();
  SerialMon.print(F("Modem: "));
  SerialMon.println(modemInfo);
  SerialMon.println(F("waitForNetwork()"));
  while (!modem.waitForNetwork()) SerialMon.print(".");
  SerialMon.println(F("gprsConnect(sl2sfr)"));
  while (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.print(".");
  }
  Serial.println();
  SerialMon.println("### Setup completed.");
  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);

}

void loop() {
  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) { lastReconnectAttempt = 0; }
    }
    delay(100);
    return;
  }

  mqtt.loop();

}
