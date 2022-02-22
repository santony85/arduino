#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <HTTPClient.h>

WiFiServer server(80);

static const char *ap_ssid = "ESP32AP";
static const char *ap_pass = "Santony85";

String serverName = "";

boolean isRpi=false;

const int buttonPinL = 12;
const int buttonPinR = 15;
const int buttonPinP = 13;

int lastButtonStateL = 1;
int lastButtonStateR = 1;
int lastButtonStateP = 1;

int buttonStateL; 
int buttonStateR; 
int buttonStateP; 

int ledState = HIGH;

int isAllume=0;

unsigned long lastDebounceTimeL = 0;  
unsigned long lastDebounceTimeR = 0;
unsigned long lastDebounceTimeP = 0;
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(buttonPinL, INPUT_PULLUP);
  pinMode(buttonPinR, INPUT_PULLUP);
  pinMode(buttonPinP, INPUT_PULLUP);
  
  Serial.begin(115200);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.print("Access point running. IP address: ");
  Serial.print(WiFi.softAPIP());
  Serial.println("");
  server.begin();
  
}

void sendGet(String text){
  HTTPClient http;
  String serverPath = serverName + "/"+text;
  Serial.println(serverPath);
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
}

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String client_ip = client.remoteIP().toString();
    Serial.print("Client connected. IP address = ");
    Serial.println(client_ip);
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println();
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        if (currentLine.endsWith("GET /H")) {
          Serial.println("Detect raspberry ip");
          serverName = "http://"+client_ip+":8080";
          isRpi=true;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  //getButtonP();
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
