#include <ESP8266WiFi.h>
int port = 4206;  //Port number
WiFiClient client;

//Server connect to WiFi Network
const char *ssid = "ESP32-BOAT";  //Enter your wifi SSID
const char *password = "Santony85";  //Enter your wifi Password


#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#define DEBUGMODE true
#define SDA 13
#define SCL 12
String Value="";
int count=0;
String MQTT_PUBLISH_Monitor="";
String myMAC;

String getMacAddress() {
  String macc=String(WiFi.macAddress());

  macc.replace(":","");
  //Serial.println(resu);  
  return macc;  
}  

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //Connect to wifi
 
  // Wait for connection  
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  myMAC = getMacAddress();
  MQTT_PUBLISH_Monitor = "distance_"+myMAC; 
  if (client.connect("192.168.4.1", port)) {
      //Serial.println("connected");
      // Make a HTTP request:
      //client.println("GET /search?q=arduino HTTP/1.0");
      client.println(MQTT_PUBLISH_Monitor);
    } 
    if (!lox.begin(0x29,SDA,SCL)) {
      Serial.println(F("Failed to boot VL53L0X"));
      while(1);
    }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);  

}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  while (client.connected()) {            // loop while the client's connected
      if (client.available()>=0) {             // if there's bytes to read from the client,
        String val = client.readString();
        Serial.println(val);  
      }
      client.println(measure.RangeMilliMeter);
    }
}