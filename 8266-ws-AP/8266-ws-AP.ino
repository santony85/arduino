#include <Adafruit_NeoPixel.h>
#define PINL        0 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 20 // Popular NeoPixel ring size

  Adafruit_NeoPixel pixelsl(NUMPIXELS, PINL, NEO_GRB + NEO_KHZ800);

  
#define DELAYVAL 20 // Time (in milliseconds) to pause between pixels

#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */
//#include <RCSwitch.h>
//RCSwitch mySwitch = RCSwitch();

#define isAP
//#define isCLIENT

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

//#define STATIC_IP                       // uncomment for static IP, set IP below
#ifdef STATIC_IP
  IPAddress ip(192,168,4,1);
  IPAddress gateway(192,168,4,1);
  IPAddress subnet(255,255,255,0);
#endif

const char *ssid = APSSID;
const char *password = APPSK;
#define WIFI_TIMEOUT 30000  

String macadd = "retrocamera";

ESP8266WebServer server(80);

int first=1;
int nbCli=0;
int isClil=0;
int isClir=0;
int isCli=0;
int isFeu=0;
int isRemote=0;
int isblk=0;
const int timeout =1000;
long int oldtime;
String ipTel="";

int rx0 = 4;    // pushbutton connected to digital pin 7
int rx1 = 5;    // pushbutton connected to digital pin 7
int vrx0 = 0;
int vrx1 = 0;

void wifi_setup() {
  /*Serial.println();
  Serial.print("Connecting to ");
  Serial.println(APSSID);*/

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
  Serial.println();}
void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
  Serial.println("\n\nGet Root...");}

void colorWipeL(){  
  for(int i=0; i<20; i++) { // For each pixel...
    pixelsl.setPixelColor(i, pixelsl.Color(255, 50, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  for(int i=0; i<20; i++) { // For each pixel...
    pixelsl.setPixelColor(i, pixelsl.Color(0, 0, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  if(nbCli <5)nbCli++;
  else {
    isClil=0;
    isCli=0;
    nbCli=0;
    isRemote=0;
    if(isFeu==1){
      feuOn();
      Serial.println("allume feu");
      }
  }}

void colorWipeR(){  
  for(int i=20; i>=0; i--) { // For each pixel...
    pixelsl.setPixelColor(i, pixelsl.Color(255, 50, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  for(int i=20; i>=0; i--) { // For each pixel...
    pixelsl.setPixelColor(i, pixelsl.Color(0, 0, 0));
    pixelsl.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  if(nbCli <5)nbCli++;
  else {
    isClir=0;
    isCli=0;
    nbCli=0;
    isRemote=0;
    if(isFeu==1){
      feuOn();
      Serial.println("allume feu");
      }
  }}
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
    oldtime=millis();
    sendToTel("m");
    }
  else {
    isFeu=0;
    feuOff();
    sendToTel("n");
    }
  isRemote=0;  
  server.send(200, "text/plain", "ok");

  }

void handleDirl(){
  Serial.println("l...");
    isClil=1;
    isCli=1;
    if(isFeu==1){
      feuOff();
      Serial.println("coupe feu");
      }
    server.send(200, "text/plain", "ok");
    sendToTel("l");
  }

void handleDirr(){
  Serial.println("r...");
    isClir=1;
    isCli=1;
    if(isFeu==1){
      feuOff();
      Serial.println("coupe feu");
    }
    server.send(200, "text/plain", "ok");
    sendToTel("r");
  }

void sendToTel(String txtToSend){
  HTTPClient http;
  WiFiClient client;
  String retval = "http://"+ipTel+":99/"+txtToSend;
  http.begin(client,retval.c_str());  //Specify request destination
  int httpCode = http.GET();                                  //Send the request
  if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
 
    }
 
    http.end();   //Close connection
  
  }

void handleIpc(){
  ipTel = server.arg(0);
  Serial.println(ipTel);
  server.send(200, "text/plain", "ok");
  }

void setup() {
  //pinMode(2, INPUT); 
  //pinMode(PINL, OUTPUT); 
  //digitalWrite(PINL,HIGH);
  //digitalWrite(2,LOW);
  delay(500);

  pixelsl.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixelsl.clear();
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\nStarting...");
  
  #ifdef isAP
    Serial.println(ssid);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.hostname(macadd.c_str());
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
  server.on("/ipc", handleIpc);
  
  server.begin();
  Serial.println("HTTP server started");
  pinMode(rx0, INPUT);    // sets the digital pin 7 as input
  pinMode(rx1, INPUT);  
  }
  
void loop() {

    server.handleClient();

    /*if(isFeu==1 && isCli==0){

      long int time = millis();      
      if(time - oldtime > timeout){
        oldtime = time;
        if(isblk == 0){
          isblk = 1;
          feuOff();  
        }
        else {
          isblk = 0;
          feuOn();
        }
      }
      
    }
    vrx0 = digitalRead(rx0);
    vrx1 = digitalRead(rx1);
    int msum = (vrx0*10)+vrx1;
    if(msum>0 && isRemote==0){
      if(msum==1){
        Serial.println("left");
        isRemote=1;
        handleDirl();
        }
      else if(msum==11){
        Serial.println("right");
        isRemote=1;
        handleDirr();
        }
      else if(msum==10){
        Serial.println("lumiere");
        isRemote=1;
        handleFeu();
        delay(1000);
        }  
      delay(100);
      }
    /*if (mySwitch.available() && isRemote==0) {
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("Bit ");
    Serial.print(" // ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    if(mySwitch.getReceivedValue() == 2354){
      isRemote=1;
      handleDirl();
      }
    else if(mySwitch.getReceivedValue() == 2359){
      isRemote=1;
      handleDirr();
      }  
    else if(mySwitch.getReceivedValue() == 2365){
      isRemote=1;
      handleFeu();
      }  
    delay(1);

    mySwitch.resetAvailable();
  }
  else mySwitch.resetAvailable();*/

  
  

  
  
  if(isClil==1 && nbCli < 6){
    colorWipeL(); 
  }
  
  if(isClir==1 && nbCli < 6){
    colorWipeR(); 
  }

}
