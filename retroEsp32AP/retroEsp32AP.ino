#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <HTTPClient.h>
#include <WebServer.h>

// Set these to your desired credentials.
const char *ssid = "ESP32AP";
const char *password = "Santony85";
const char* host = "192.168.4.2";

WebServer server(80);
WiFiClient client;

String client_ip="";
String serverName = "";

boolean isConnectedRpi=false;

int mode=0;
//mode 0 normal
//mode 1 menu

int valuex = 0;
int valuey = 0;
int initx = 0;
int inity = 0;

int seuil=1;

int iswaitx=0;
int iswaity=0;
int isAllume=0;
int isMap=0;
int isbuzy=0;

int cligno=A3;
int light=A0;

void handleRoot() {
 // WiFiClient client;
  client_ip = server.client().remoteIP().toString();
  Serial.print("Client connected. IP address = ");
  Serial.println(client_ip);
  server.send(200, "text/html", "<h1>You are connected</h1>");
  Serial.println("Detect raspberry ip");
  serverName = "http://"+client_ip+":8080";
  isConnectedRpi=true;
}

void setup() {
  pinMode(A3, INPUT);
	pinMode(A0, INPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  // Configuring access point...
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("Server started");
  Serial.println("HTTP server started");
  /////********************
  server.begin();
  server.on("/H", handleRoot);
  
  
  initx = analogRead(cligno) * (5.0 / 1023.0); 
  inity = analogRead(light) * (5.0 / 1023.0); 

  Serial.println(initx); 
  Serial.println(inity); 

}

void sendGet(String text){
  HTTPClient http;
  String serverPath = serverName + "/"+text;
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
        String payload = http.getString();
      }
  else {
        //Serial.print("Error code: ");
        //Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
  //delay(5000);
  
}

void loop() {

  server.handleClient();
  if(isConnectedRpi){
    if(mode==0){



  if(iswaity == 0)getCligno(); 
  if(iswaitx == 0)getLight();


    }
    //else if(mode==1)
    //else
  } 


  

  delay(100);
}

void getCligno() {
  valuex = analogRead(cligno) * (5.0 / 1023.0); 	
  int tmpv = abs(initx-valuex);

  if(tmpv > seuil && iswaitx == 0){
    if(valuex > initx){
      Serial.println("rw");
      iswaitx=1;
      sendGet("rw");
    }
    else {
      Serial.println("lw");
      iswaitx=1; 
      sendGet("lw");
    }
    delay(500);
    isbuzy=1;
  }
  else if(tmpv < seuil && iswaitx == 1){
    Serial.println("ok"); 
    iswaitx=0;
    isbuzy=0;
    delay(500);
  }
}

void getLight() {
  valuey = analogRead(light) * (5.0 / 1023.0);	
  int tmpv = abs(inity-valuey);
  if(tmpv > seuil && iswaity == 0 ){
    /* for light */
    if(valuey < inity){
      iswaity=1;
      if(!isAllume){
        Serial.println("light on");
        isAllume =1;
        sendGet("mw");
        }
    
      else {
        Serial.println("light off");
        isAllume =0;
        sendGet("nw");
        }
    }
    /* for map */
    else {
      iswaity=1; 
      if(!isMap){
        Serial.println("map on");
        isMap =1;
        sendGet("ms");
        }
      else {
        Serial.println("map off");
        isMap =0;
        sendGet("mh");
        }

    }
    delay(500);
    isbuzy=1;  
  }
  else if(tmpv < seuil && iswaity == 1){
    Serial.println("ok"); 
    iswaity=0;
    isbuzy=0;
    delay(500);
  }

}

