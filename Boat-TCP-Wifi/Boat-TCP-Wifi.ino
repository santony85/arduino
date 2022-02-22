
#include <stdio.h>
#include <string.h>
#define ssGSM Serial2
int RST_KEY = 15;
bool ModuleState = false;
String myMacAdr;

#include "libGSM.h"
String gsmIp;

#include <WiFi.h>
#include <ESPmDNS.h>
#include "TinyMqtt.h"  
#include <base64.h>

//#include <Arduino.h>
#include "FS.h"
#include <LITTLEFS.h>
#define FORMAT_LITTLEFS_IF_FAILED true
#include "filemanage.h"

#define PORT 1883
MqttBroker broker(PORT);
MqttClient mqtt_a(&broker);

const char *ssid = "ESP32-BOAT";
const char *passphrase = "Santony85";

int cnt=0;

IPAddress local_IP(192,168,4,1);
IPAddress subnet(255,255,255,0);
IPAddress gateway(192,168,4,1);

int isInit=0;
int isGPS=0;
String mname;

std::string topic="camera";

#define ARRAYSIZE 128
String lstPeriph[ARRAYSIZE];
int arrayPos=0;
int waitPing=0;


int onPublishA(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t  plength)
{ 
  String topp = topic.c_str();
  int sz = plength;
  Serial.println(topp);
  if(topic == std::string("camera") && payload != "OK"){
    Serial.println("New Connection");
    lstPeriph[arrayPos++] = String(payload);
    mqtt_a.subscribe(payload);
    String vt = "command_"+String(payload);
    mqtt_a.subscribe(vt.c_str());
    connectGSM("AT+MQTTSUB=\""+String(payload)+"\",1,0","OK");
    //mqtt_a.publish("camera_A8032A1462A0", "connected");
    Serial << "--> server received " << topic.c_str() << ", " << payload << endl; 
    isInit=1;
    }
    
  if(topp.indexOf("commande_")>=0){
    
    /*Serial.print("sz: ");Serial.println(sz);
    Serial.println(topp);
    if(topp.indexOf("camera")>=0){
      String base64data = base64::encode((uint8_t*)payload, sz);
      connectGSM("AT+MQTTPUB=\""+topp+"\",\""+base64data+"\",0,0,0","",false);*/
    }
  else if(topp.indexOf("camera_")>=0){
    Serial.print("sz: ");Serial.println(sz);
    
    if(topp.indexOf("camera")>=0){
      String base64data = base64::encode((uint8_t*)payload, sz);
      connectGSM("AT+MQTTPUB=\""+topp+"\",\""+base64data+"\",0,0,0","",false);
    }
    //
    
    //String pl=String(payload);
    //connectGSM("AT+MQTTPUB=\""+topp+"\",\""+payload+"\",0,0,0","",false);
    
    //Serial << "--> server received " << topic.c_str() << ", " << payload << endl;
    //  
  }
  /*else if(topic == std::string(lstPeriph[0].c_str())){
    Serial.println("get data");
  }  */
  
  //mqtt_a.publish(lstCamera.c_str(), "OK");
  //return 0;
}

String JSONlistPeriph() {
  //double dst = CalcDistance(alflat, alflon, flat, flon );
  String res="{'periphs':[";   
    for(int i=0;i<arrayPos;i++){
      res = res+"{'topic':'"+lstPeriph[i]+"',";
      res = res+"'status':'"+lstPeriph[i]+"'},";
    }
  if(arrayPos >0)res = res.substring(0, res.length() - 1);  
  res = res+"]}";
  Serial.println(res);  
  return res;

}

void setup() {
  pinMode(RST_KEY, OUTPUT);
  Serial.begin(115200);
  Serial.println("Start");
  ssGSM.begin(115200);
  waitReady();
  Serial.println("MODEM Ready");
  initSMS();
  Serial.println("SMS Ready");
  //initGPRS();
  initGPRSMQTT();
  Serial.println("GPRS Ready");
  Serial.println("A9G init OK");


  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,passphrase) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  //initGPS("7");
  Serial.println("GPS started");
  broker.begin();
  Serial << "Broker ready : " << WiFi.softAPIP() << " on port " << PORT << endl;
  mqtt_a.setCallback(onPublishA);
  mqtt_a.subscribe(topic);
  String myMAC = getMacAddress();
  mname = "boat_"+myMAC;

  }

void loop() {

  broker.loop();
  mqtt_a.loop();
  if(isInit){
    isInit=0; 
    delay(2000);
    mqtt_a.publish(lstPeriph[arrayPos-1].c_str(), "OK");   
  }

  /*String msgIn,gsmIn;
  if (Serial.available() > 0) {
    msgIn = Serial.readString();
    ssGSM.println(msgIn);
  }
  if (ssGSM.available() > 0){
    gsmIn = ssGSM.readString();
    Serial.println(gsmIn);  
  }*/
  //  subscribe callback  
  String sub_message = "";  
  long int subtime = millis();
  while (subtime+300>millis()){
    while(ssGSM.available() > 0) {     
        char c = ssGSM.read();
        sub_message += c;
      }
  }
  if (sub_message != "")Serial.println(sub_message);
  if (sub_message.indexOf("getlist")>-1) {
    String val=JSONlistPeriph();
    connectGSM("AT+MQTTPUB=\""+mname+"\",\""+val+"\",0,0,0","",false);  
    }
  /*else if (sub_message.indexOf(lstPeriph[0])>-1) {
    
    //String val=JSONlistPeriph();
    //connectGSM("AT+MQTTPUB=\""+mname+"\",\""+val+"\",0,0,0",""); 
    }*/  
  else if (sub_message.indexOf("start")>-1) {
    String val = sub_message;
    Serial.println("ici");
    String vtopic = "command_"+getValue(val,',', 1);  
    Serial.println(vtopic);
    String command=getValue(val,',', 3);  
    Serial.println(command);
    mqtt_a.publish(vtopic.c_str(), command);
  }
  else if (sub_message.indexOf("stop")>-1) {
    String val = sub_message;
    String vtopic = "command_"+getValue(val,',', 1);  
    Serial.println(vtopic);
    String command=getValue(val,',', 3);  
    Serial.println(command);
    mqtt_a.publish(vtopic.c_str(), command);
  }
  else if (sub_message.indexOf("nextf")>-1) {
    String val = sub_message;
    String vtopic = "command_"+getValue(val,',', 1);  
    Serial.println(vtopic);
    String command=getValue(val,',', 3);  
    Serial.println(command);
    mqtt_a.publish(std::string("command_camera_A8032A1462A0"), command);
    delay(100);
  }     
  else if (sub_message.indexOf("setGPSOn")>-1) {
    Serial.println("gps ON!");
    isGPS=1;
  }
  else if (sub_message.indexOf("setGPSOff")>-1) {
    Serial.println("gps OFF!");
    isGPS=0;
  }
  

  

}
