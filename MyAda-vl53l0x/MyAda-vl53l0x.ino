
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial

#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//#define DEBUGMODE false

#define SDA 13
#define SCL 12
String MQTT_PUBLISH_Monitor="";
String MQTT_PUBLISH_Command="";
String myMAC="";
int isStream=0;
int count=0;
String data;
String cap ="tachometer";

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  String text =(char*)payload;
	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      if(text.indexOf("connected")>=0){
        USE_SERIAL.println("cnt Ok");
        webSocket.sendTXT("cap@"+cap);//send service
      }
      else if(text.indexOf("start")>=0){
        USE_SERIAL.println("Start Ok");
        isStream=1;
      }
      else if(text.indexOf("stop")>=0){
        USE_SERIAL.println("Stop Ok");
        isStream=0;
      }
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

void setup() {
  USE_SERIAL.begin(115200);
  if (!lox.begin(0x29,SDA,SCL)) {
      USE_SERIAL.println(F("Failed to boot VL53L0X"));
    while(1);
    }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);
  USE_SERIAL.println("Start");

  WiFiMulti.addAP("ESP32-BOAT", "Santony85");
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}
  
  myMAC = String(WiFi.macAddress());
  myMAC.replace(":","");
  MQTT_PUBLISH_Monitor="distance_"+myMAC;
	webSocket.begin("192.168.4.1", 4206, MQTT_PUBLISH_Monitor);
	webSocket.onEvent(webSocketEvent);
	webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);
  //setup_wifi();
  if (!MDNS.begin(MQTT_PUBLISH_Monitor.c_str())) {             // Start the mDNS responder for esp8266.local
    USE_SERIAL.println("Error setting up MDNS responder!");
  }
  USE_SERIAL.println("mDNS responder started");
}


void loop() {
  webSocket.loop();
  VL53L0X_RangingMeasurementData_t measure;
  //Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    data=String(measure.RangeMilliMeter);
    #ifdef DEBUGMODE 
      Serial.println(measure.RangeMilliMeter);
    #endif 
  } else {
    #ifdef DEBUGMODE
      Serial.println(" out of range ");
    #endif 
  }
    if(isStream){
    webSocket.sendTXT(data);
    delay(100);
    count++;
    if(count > 15){
       count=0;
      isStream=0; 
      }    
    }
    
  delay(100);
}
