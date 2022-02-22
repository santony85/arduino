#include <Adafruit_NeoPixel.h>
#define PINL        0 // On Trinket or Gemma, suggest changing this to 1
#define PINR        2 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 40 // Popular NeoPixel ring size
#include "lines.h"
  Adafruit_NeoPixel pixelsl(NUMPIXELS, PINL, NEO_GRB + NEO_KHZ800);

  
#define DELAYVAL 50 // Time (in milliseconds) to pause between pixels

#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */


//#define isAP
#define isCLIENT

#ifdef isAP
  #define APSSID "RetroAP8266"
  #define APPSK  "Santony85"
  IPAddress local_IP(192,168,4,1);
  IPAddress gateway(192,168,4,1);
  IPAddress subnet(255,255,255,0);

#endif

#ifdef isCLIENT
  #define APSSID "Livebox-4296"
  #define APPSK  "Santony85"
#endif

#define STATIC_IP                       // uncomment for static IP, set IP below
#ifdef STATIC_IP
  IPAddress ip(192,168,1,123);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
#endif

const char *ssid = APSSID;
const char *password = APPSK;
#define WIFI_TIMEOUT 30000  


ESP8266WebServer server(80);

int first=1;
int nbCli=0;
int isClil=0;
int isClir=0;
int isFeu=0;


void wifi_setup() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(APSSID);

  WiFi.begin(APSSID, APPSK);
  WiFi.mode(WIFI_STA);
  #ifdef STATIC_IP  
    WiFi.config(ip, gateway, subnet);
  #endif

  unsigned long connect_start = millis();
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    if(millis() - connect_start > WIFI_TIMEOUT) {
      Serial.println();
      Serial.print("Tried ");
      Serial.print(WIFI_TIMEOUT);
      Serial.print("ms. Resetting ESP now.");
      ESP.reset();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void handleRoot() {
 // server.send(200, "text/plain", "hello from esp8266!");
  Serial.println("\n\nGet Root...");
  String message = "Number of args received:";
    message += server.args();            //Get number of parameters
    message += "\n";                            //Add a new line

for (int i = 0; i < server.args(); i++) {

message += "Arg nº" + (String)i + " –> ";   //Include the current iteration value
message += server.argName(i) + ": ";     //Get the name of the parameter
message += server.arg(i) + "\n";              //Get the value of the parameter

} 

server.send(200, "text/plain", message);       //Response to the HTTP request
  
}

void colorWipeL(){  
  for(int i=0; i<41; i++) { // For each pixel...
    pixelsl.setPixelColor(i, pixelsl.Color(255, 50, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  for(int i=0; i<40; i++) { // For each pixel...
    pixelsl.setPixelColor(i, pixelsl.Color(0, 0, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  if(nbCli <5)nbCli++;
  else {
    /*isClil=0;
    nbCli=0;
    if(isFeu==1){
      feuOn();
      Serial.println("allume feu");
      }*/
  }
}

void colorWipeR(){  
  for(int i=0; i<10; i++) { // For each pixel...
    pixelsl.setPixelColor(i+10, pixelsl.Color(255, 50, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  for(int i=0; i<10; i++) { // For each pixel...
    pixelsl.setPixelColor(i+10, pixelsl.Color(0, 0, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  if(nbCli <5)nbCli++;
  else {
    isClir=0;
    nbCli=0;
    if(isFeu==1){
      feuOn();
      Serial.println("allume feu");
      }
  }
}

void feuOn(){
    pixelsl.setPixelColor(0, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(2, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(4, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(6, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(8, pixelsl.Color(255, 0, 0)); 
    pixelsl.setPixelColor(10, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(12, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(14, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(16, pixelsl.Color(255, 0, 0));
    pixelsl.setPixelColor(18, pixelsl.Color(255, 0, 0)); 
    pixelsl.show();}

void feuOff(){
    pixelsl.setPixelColor(0, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(2, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(4, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(6, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(8, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(10, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(12, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(14, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(16, pixelsl.Color(0, 0, 0));
    pixelsl.setPixelColor(18, pixelsl.Color(0, 0, 0));
    pixelsl.show();
    }

void handleFeu(){
  Serial.println("m...");
  if(isFeu==0){
    isFeu=1;
    feuOn();
    }
  else {
    isFeu=0;
    feuOff();
    }
  server.send(200, "text/plain", "ok");

  }

void handleDirl(){
  Serial.println("l...");
    isClil=1;
    if(isFeu==1){
      feuOff();
      Serial.println("coupe feu");
      }
    server.send(200, "text/plain", "ok");
  }

void handleDirr(){
  Serial.println("r...");
    isClir=1;
    if(isFeu==1){
      feuOff();
      Serial.println("coupe feu");
    }
    server.send(200, "text/plain", "ok");
  }

void affBand(){
  for(int i=0;i<41;i++){
    int a = logo_bits[i*3];
    int b = logo_bits[(i*3)+1];
    int c = logo_bits[(i*3)+2];
    pixelsl.setPixelColor(i, pixelsl.Color(a, b, c));
    }
  pixelsl.show(); 
}

void setup() {
  pinMode(2, OUTPUT); 
  pinMode(0, OUTPUT); 
  digitalWrite(0,LOW);
  digitalWrite(2,LOW);
  delay(500);

  pixelsl.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixelsl.clear();
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\nStarting...");
  
  #ifdef isAP
    Serial.println(ssid);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  #endif
  #ifdef isCLIENT
      Serial.println(password);
      wifi_setup();
  #endif
  server.on("/", handleRoot);
  server.on("/l", handleDirl);
  server.on("/r", handleDirr);
  server.on("/m", handleFeu);
  
  server.begin();
  Serial.println("HTTP server started");
  }
  
void loop() {

  server.handleClient();

  affBand();
  delay(500);
  /*if(isClil==1 && nbCli < 6){
    colorWipeL(); 
  }
  
  if(isClir==1 && nbCli < 6){
    colorWipeR(); 
  }*/

}
