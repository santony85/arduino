#include <ESP8266WiFi.h>
int port = 4206;  //Port number
WiFiClient client;

//Server connect to WiFi Network
const char *ssid = "Livebox-4296";  //Enter your wifi SSID
const char *password = "Santony85";  //Enter your wifi Password
IPAddress server(51,68,125,223);

#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#define DEBUGMODE true
#define SDA 13
#define SCL 12
String Value="";
int count=0;
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
      if (client.connect(server, 3333)) {
      //Serial.println("connected");
      // Make a HTTP request:
      //client.println("GET /search?q=arduino HTTP/1.0");
      client.println("Hello");
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

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
          client.println(measure.RangeMilliMeter);
          Value = measure.RangeMilliMeter;
          Serial.println(measure.RangeMilliMeter);
        } else {

        client.println(0);

        }

  if (!client.connected()) {
        //try to connect to a new client
        if(client.connect(server, 3333)){}
        else Serial.println("not connected");
    } else {
        //read data from the connected client
        if (client.available() > 0) {
            Serial.write(client.read());
        }

    }


  delay(100);

}
