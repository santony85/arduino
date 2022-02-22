#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include "LittleFS.h"

#define APSSID "raspicuve"
//#define APSSID "Livebox-4296"
#define APPSK  "Santony85"

ESP8266WebServer server(80);

#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

float temp;     // actual temperature variable
int vmin = 20;
int vmax = 26;
int   vnum = 9;
String vname = "Change Cuve";
int address=0;
int alarmeencours=0;
int relaison=0;


String myIp;

int Buzzer = 18;
int Relais = 13;//13
int vout=5;
int isOnOff = 1;
int isForce = 0;

unsigned long previousMillis  = 0;
unsigned long currentMillis   = 0;

int interval = 1000;



const char* PARAM_1 = "vmin";
const char* PARAM_2 = "vmax";
const char* PARAM_3 = "vnum";
const char* PARAM_4 = "vname";
const String inputParam1 = "vmin";
const String inputParam2 = "vmax";
const String inputParam3 = "vnum";
const String inputParam4 = "vname";

void(* resetFunc) (void) = 0;
//resetFunc();

String IpAddress2String(const IPAddress& ipAddress){
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}


void setup(){
  // start the serial port at 9600 baud
  Serial.begin(115200);
  delay(500);
   Serial.println();
   Serial.print("MAC: ");
   
   String macadd = WiFi.macAddress();
   macadd.replace(":", "");
   macadd= "ESP-"+macadd;
   Serial.println(macadd);

  pinMode (Buzzer, OUTPUT);
  digitalWrite(Buzzer,HIGH);
  pinMode (Relais, OUTPUT);
  digitalWrite(Relais,HIGH);



  Serial.println("Start setting");

  
   
  
  WiFi.mode(WIFI_STA);
  WiFi.hostname(macadd.c_str());
  WiFi.begin(APSSID, APPSK);  
  
   
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {  // Wait until WiFi is connected
    delay(500);
    Serial.print(".");
    }
  Serial.println();
  //Serial.println("Connected to the WiFi network"); // Print wifi connect message
  Serial.println();
  Serial.print("IP Address: ");
  myIp=IpAddress2String(WiFi.localIP());
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  // Send web page with input fields to client
  

  server.begin();

  server.on("/",handleRoot);
  server.on("/temp/"+myIp,handleTemp);
  server.on("/set/"+myIp,handleSet);
  server.on("/setonoff/"+myIp,handleOnOff);
  server.on("/reset/"+myIp,handleReset);
  server.on("/force/"+myIp,handleForce);
  
  pinMode (vout, OUTPUT);
  digitalWrite(vout,HIGH);
  sensors.begin();
  delay(2000);
  
  if(!LittleFS.begin()){
     Serial.println("An Error has occurred while mounting LittleFS");
     delay(1000);
     return;
  }
  File file = LittleFS.open("/cuve.txt", "r");
  if(!file){
    Serial.println("No Saved Data!");
    writeData(makeString());
   }
  readData();
  delay(3000);
  
  }

String makeString(){
    String valt="";
    valt += vnum;
    valt +=";";
    valt += vmin;
    valt +=";";
    valt += vmax;
    valt +=";";
    valt += vname;
    valt +=";";
    valt += isOnOff;
    valt +=";";
    valt += myIp;
    Serial.println(valt);
    return valt;
  }

void readData(){
  File file = LittleFS.open("/cuve.txt", "r");
   String val="";
   while(file.available()){
      char xx = (char)file.read();
      val.concat(xx);
   }
   file.close();
   //Serial.println(val);
   //split string;
    char buf[1024];
    val.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    int a=0;
    while ((str = strtok_r(p, ";", &p)) != NULL){ // delimiter is the semicolon
      if(a==0)vnum=atoi(str);
      if(a==1)vmin=atoi(str);
      if(a==2)vmax=atoi(str);
      if(a==3)vname=str;
      if(a==4)isOnOff=atoi(str);
      if(a==5)myIp=str;
      //Serial.print(a++);
      //Serial.print("->");
      //Serial.println(str);
      a++;
    }
   Serial.println(vname);
   Serial.println(vmax);
   Serial.println(vmin);
   Serial.println(vnum);
   Serial.println(isOnOff);
   Serial.println(myIp);
  }
  
void writeData(String data){
  File file = LittleFS.open("/cuve.txt", "w");
  file.print(data);
  delay(1);
  file.close();
  }
  
void handleOnOff(){
  Serial.println("/setonoff");
  isOnOff = !isOnOff;
  server.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; URL=/'></head><body></body></html>"); 
}

void handleReset(){
   server.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; URL=/'></head><body></body></html>"); 
   resetFunc();
  }
  
void handleRoot(){
  String retval = String(temp)+";"+String(alarmeencours)+";"+String(relaison)+";"+String(isOnOff)+";"+String(vname)+";"+String(vmax)+";"+String(vmin)+";"+String(vnum);
  server.send(200, "text/plain", retval.c_str());
  }


void handleTemp(){
    String retval = String(temp)+";"+String(alarmeencours)+";"+String(relaison)+";"+String(isOnOff)+";"+String(vname)+";"+String(vmax)+";"+String(vmin)+";"+String(vnum);
    server.send(200, "text/plain", retval.c_str());
  }

void handleSet(){
    Serial.println("/set");
    vname = server.arg(0);
    vmax  = server.arg(1).toInt();
    vmin  = server.arg(2).toInt();
    vnum  = server.arg(3).toInt();
    writeData(makeString());
    server.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; URL=/'></head><body></body></html>"); 
}

void readTemperature() {
  digitalWrite(vout,HIGH);
  delay(200);
  sensors.requestTemperatures(); 
  temp = sensors.getTempCByIndex(0);
  Serial.println(temp);
  digitalWrite(vout,LOW);
  }
  
void automate(){
  if(isOnOff){
    //***** ALARME *****
    if(temp > vmax){
      if(alarmeencours==0){
        alarmeencours=1;
        }
      digitalWrite (Buzzer, LOW); //turn buzzer on
      delay(100);
      digitalWrite (Buzzer, HIGH);  //turn buzzer off
      delay(100);
      }
    //***** RELAIS ****
    else if(temp > (vmin+1) && temp < vmax){
      //relais on 
      alarmeencours=0;
      relaison=1;
      digitalWrite(Relais,LOW);
      }
    else if(temp < vmin-1 && temp != -127){
      //relais off 
      relaison=0;
      digitalWrite(Relais,HIGH);
      if(alarmeencours==0){
        alarmeencours=1;
        } 
      } 
    else {
      digitalWrite(Relais,HIGH);
      relaison=0;
      if(alarmeencours==1)alarmeencours=0;
      } 
    } 
  else{
    //Serial.println("is off");
    alarmeencours=0;
    digitalWrite (Buzzer, HIGH);  //turn buzzer off
    digitalWrite (Relais, HIGH);  //turn buzzer off
  }
}

void loop() {
  server.handleClient();
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval == true ) {
      previousMillis = currentMillis;
      readTemperature();
      automate();
  }
  
}
