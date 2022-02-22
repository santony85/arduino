/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>

const char* ssid     = "Livebox-4296";
const char* password = "Santony85";

const char* host = "retrocamera";
const char* streamId   = "....................";
const char* privateKey = "....................";
WiFiClient client;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

#define btl           9           // Configurable, see typical pin layout above
#define btr          10 
#define btp          11 

bool isConnected= false;

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

int value = 0;

void settingWifi(String action){
  //delay(5000);
  //  ++value;
    Serial.print("connecting to ");
    Serial.println(host);
    // Use WiFiClient class to create TCP connections
    const int httpPort = 8080;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }
    // We now create a URI for the request
    String url = "/"+action;
    Serial.print("Requesting URL: ");
    Serial.println(url);
    // ok
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
        
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
    Serial.println();
    Serial.println("closing connection");
    isConnected= true;
  }
void loop()
{
  delay(5000);
  settingWifi("lw");
    delay(5000);
  settingWifi("rw");
  delay(5000);
  settingWifi("mw");
    delay(5000);
  settingWifi("nw");

}
