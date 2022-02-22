#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#define APSSID "raspicuve"
//#define APSSID "Livebox-4296"
#define APPSK  "Santony85"

ESP8266WebServer server(80);

int vout=16;

  IPAddress ip(192,168,1,200);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

void setup(){
  Serial.begin(115200); 
  pinMode (vout, OUTPUT);
  digitalWrite(vout,LOW); 


  WiFi.begin(APSSID, APPSK); 
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet); 

  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {  // Wait until WiFi is connected
    delay(500);
    Serial.print(".");
    }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  server.begin();
  server.on("/reset",handleReset);
 
  
  
  }

  void handleReset(){
    Serial.println("reset");
    server.send(200, "text/html", "ok");
    digitalWrite(vout,HIGH);
    delay(4000);
    digitalWrite(vout,LOW);
  }

void loop(){

  server.handleClient();
  }
