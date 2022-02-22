

#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#ifndef APSSID
#define APSSID "ESP32AP"
#define APPSK  "Santony85"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

String serverName = "";

boolean isRpi=false;

const int buttonPinL = 0;
const int buttonPinR = 2;
const int buttonPinP = 3;

int lastButtonStateL = 1;
int lastButtonStateR = 1;
int lastButtonStateP = 1;

int buttonStateL; 
int buttonStateR; 
int buttonStateP; 

int ledState = HIGH;

int isAllume=0;

String client_ip="";

unsigned long lastDebounceTimeL = 0;  
unsigned long lastDebounceTimeR = 0;
unsigned long lastDebounceTimeP = 0;
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
 // WiFiClient client;
  client_ip = server.client().remoteIP().toString();
  Serial.print("Client connected. IP address = ");
  Serial.println(client_ip);
  server.send(200, "text/html", "<h1>You are connected</h1>");
  Serial.println("Detect raspberry ip");
  serverName = "http://"+client_ip+":8080";
  isRpi=true;
}

void setup() {
  delay(1000);
  pinMode(buttonPinL, INPUT_PULLUP);
  pinMode(buttonPinR, INPUT_PULLUP);
  pinMode(buttonPinP, INPUT_PULLUP);
  
  Serial.begin(115200/*,SERIAL_8N1,SERIAL_TX_ONLY*/);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/H", handleRoot);
  server.begin();

  
  Serial.println("HTTP server started");
}

void sendGet(String text){
  HTTPClient http;
  String serverPath = serverName + "/"+text;
  //Serial.println(serverPath);
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        String payload = http.getString();
        //Serial.println(payload);
      }
      else {
        //Serial.print("Error code: ");
        //Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    

}
void loop() {
  server.handleClient();
  if(isRpi){
    getButtonL();
    getButtonR();
    getButtonP();
  }
}

void getButtonL(){
  int reading = digitalRead(buttonPinL);
  //Serial.print(reading);
  //Serial.print(" ");
  //Serial.println(lastButtonStateL);
  if (reading != lastButtonStateL) {
    lastDebounceTimeL = millis();
  }
  if ((millis() - lastDebounceTimeL) > debounceDelay) {
    if (reading != buttonStateL) {
      buttonStateL = reading;
       Serial.println("L");
      if (buttonStateL == HIGH) {
          sendGet("lw");
      }
    }
  }
  // set the LED:
  lastButtonStateL = reading;
}

void getButtonR(){
  int reading = digitalRead(buttonPinR);
  //Serial.print(reading);
  //Serial.print(" ");
  //Serial.println(lastButtonStateR);
  if (reading != lastButtonStateR) {
    lastDebounceTimeL = millis();
  }
  if ((millis() - lastDebounceTimeL) > debounceDelay) {
    if (reading != buttonStateR) {
      buttonStateR = reading;
       Serial.println("R");
      if (buttonStateR == HIGH) {
          sendGet("rw");
      }
    }
  }
  // set the LED:
  lastButtonStateR = reading;
}

void getButtonP(){
  int reading = digitalRead(buttonPinP);
  //Serial.print(reading);
  //Serial.print(" ");
  //Serial.println(lastButtonStateP);
  if (reading != lastButtonStateP) {
    lastDebounceTimeP = millis();
  }
  if ((millis() - lastDebounceTimeP) > debounceDelay) {
    if (reading != buttonStateP) {
      buttonStateP = reading;
      if (buttonStateP == LOW) {
          if(!isAllume){
            sendGet("mw");
            isAllume =1;
          }
          else {
            sendGet("nw");
            isAllume =0;
          }
      }
    }
  }
  lastButtonStateP = reading;
}
