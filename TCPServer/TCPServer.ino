#include <stdio.h>
#include <string.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <base64.h>
int port = 4206;  //Port number
#include <MicroNMEA.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>

int isEnable=1;
int isListenGPS=0;
int isListenMPU=0;
String mname;
String phone[5] = {"+33659401246", " ", " ", " ", " "};
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
int RST_KEY = 15;
#define ssGSM Serial2

#include "libGSM.h"
#include "mynmea.h"
#include "myJY901.h"


//#define wifiap true
#define wifista true
#define usemodem true

  IPAddress local_IP(192,168,4,1);
  IPAddress subnet(255,255,255,0);
  IPAddress gateway(192,168,4,1);
  
#ifdef wifiap
  const char *ssid = "ESP32-BOAT";
  const char *passphrase = "Santony85";
  wifi_mode_t wmode=WIFI_AP;
#endif
#ifdef wifista
  const char *ssid = "Livebox-4296";
  const char *passphrase = "Santony85";
  wifi_mode_t wmode=WIFI_STA; 
  WiFiMulti WiFiMulti;
#endif
WebSocketsServer bwebsocket = WebSocketsServer(4206);

#define USE_SERIAL Serial
#define ARRAYSIZE 128
String lstPeriph[ARRAYSIZE];
String lstService[ARRAYSIZE];
String lstIp[ARRAYSIZE];
int arraySize=0;

String myMAC;
unsigned long last_10sec = 0;
unsigned long last_1sec=0;
unsigned int counter = 0;

#include <SPIFFS.h>
#include <fstream>

String cap="phone,globe,anchor,compass,database";

char myName[] = "ESP32 camera server";
bool otaEnabled = true;



void initOTA() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    TelnetStream.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
    Serial.println();

    TelnetStream.printf("Progress: %u%%\r\n", (progress / (total / 100)));
    TelnetStream.println();
    
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
      TelnetStream.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
      TelnetStream.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
      TelnetStream.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
      TelnetStream.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
      TelnetStream.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

int getIdByName(String sName){
  int nb = bwebsocket.connectedClients(1);
  for(int i=0;i<nb;i++){
    if(lstPeriph[i]==sName){
      return i;
    }
  }
}

void boatWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    String text="";
    String base64data="";
    int sz =0;
    if(length < 64)text=(char*)payload;
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            //connectGSM("AT+MQTTSUB=\""+lstPeriph[num]+"\",1,0","OK");
            break;
        case WStype_CONNECTED:
            {
            IPAddress ip = bwebsocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            lstPeriph[num] = text;
            lstIp[num] = String(ip);
            arraySize=num+1;
            bwebsocket.sendTXT(num, "connected");
            connectGSM("AT+MQTTSUB=\""+lstPeriph[num]+"\",1,0","OK");
            USE_SERIAL.print("c->");
            USE_SERIAL.println(lstPeriph[num]);
            
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
            if(text.indexOf("start")>=0){
              USE_SERIAL.println("Start Ok");
             }
            //add services;
            else if(text.indexOf("cap@")>=0){
              USE_SERIAL.println("cap Ok");
              lstService[num]=getValue(text, '@', 1);
             }
            
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            //decode trame
            //From ?
            base64data = base64::encode(payload, length);
            sz = strlen(base64data.c_str());
            //Serial.println(base64data);
            connectGSM("AT+MQTTPUB=\""+lstPeriph[num]+"\",\""+base64data+"\",0,0,0","",false); 
            break;
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    }

}

String JSONlistPeriph() {
  //double dst = CalcDistance(alflat, alflon, flat, flon );
  int nb = bwebsocket.connectedClients(1);
  String res="{'periphs':[";   
  res = res+"{'topic':'"+mname+"',";
  res = res+"'status':'"+cap+"',";
  res = res+"'ip':'192.168.4.1'},";
    for(int i=0;i<nb;i++){
      res = res+"{'topic':'"+lstPeriph[i] +"',";
      res = res+"'status':'"+lstService[i]+"',";
      res = res+"'ip':'"+lstIp[i]+"'},";
    }
  res = res.substring(0, res.length() - 1);  
  res = res+"]}";
  Serial.println(res);  
  return res;

}

void wifiSetup(){
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,passphrase) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}

void setup() {
  pinMode(RST_KEY, OUTPUT);
  Serial.begin(115200);
  Serial.println("Start");
  ssGSM.begin(115200);
  #ifdef wifiap
    //WiFi.mode(wmode);
    wifiSetup();
  #endif 
  #ifdef wifista
     WiFi.mode(wmode);
     WiFiMulti.addAP(ssid, passphrase);
    while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
    }
    Serial.println(WiFi.localIP());
  #endif 
  if (otaEnabled) {
        // Start OTA once connected
        Serial.println("Setting up OTA");
        // Port defaults to 3232
        // ArduinoOTA.setPort(3232); 
        // Hostname defaults to esp3232-[MAC]
        ArduinoOTA.setHostname(myName);
        // No authentication by default
        ArduinoOTA
            .onStart([]() {
              String type;
              if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
              else // U_SPIFFS
                type = "filesystem";
              // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
              Serial.println("Start updating " + type);
            })
            .onEnd([]() {
              Serial.println("\r\nEnd");
            })
            .onProgress([](unsigned int progress, unsigned int total) {
              Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
            })
            .onError([](ota_error_t error) {
              Serial.printf("Error[%u]: ", error);
              if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
              else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
              else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
              else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
              else if (error == OTA_END_ERROR) Serial.println("End Failed");
            });
        ArduinoOTA.begin();
    } else {
        Serial.println("OTA is disabled");
    }
    
  bwebsocket.begin();
  bwebsocket.onEvent(boatWebSocketEvent);
  TelnetStream.begin();
  
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.softAPIP());
  Serial.print(" on port ");
  Serial.println(port);

  myMAC = String(WiFi.macAddress());
  myMAC.replace(":","");
  mname = "boat_"+myMAC;
  Serial.println("Wifi READY");
  Serial.println("Start JY901");
  JY901.startIIC();

  //#ifdef usemodem
    Serial.println("Start GPRS");
    waitReady();
    Serial.println("MODEM Ready");
    initSMS();
    Serial.println("SMS Ready");
    //initGPRS();
    initGPRSMQTT();
  //#endif
  
  Serial.println("GPRS Ready");
  Serial.println("A9G init OK"); 
}

void sendSocketCommand(String val){
    String vtopic = getValue(val,',', 1);  
    Serial.println(vtopic);
    String command=getValue(val,',', 3);  
    Serial.println(command);
    if(vtopic != mname){
      Serial.print("is :");
      int sender=getIdByName(vtopic);
      Serial.println(sender);
      bwebsocket.sendTXT(sender, command);
    }
    else{
      Serial.println(val);
    }
 
}

void loop() {
  ArduinoOTA.handle();
  bwebsocket.loop();
    makeMPUData();  

  #ifdef usemodem
  String msgIn;
  if (Serial.available() > 0) {
      msgIn = Serial.readString();
      ssGSM.println(msgIn);
    }
  // manage mqtt from server
  String sub_message = "";  
  long int subtime = millis();
  if(isEnable){

  while (subtime+150>millis()){
    while(ssGSM.available() > 0) {     
        char c = ssGSM.read();
        sub_message += c;
      }
  }
  //handle sms
  if (sub_message.indexOf("#info") >= 0) {
      //ModuleState = false;
      //ssGSM.flush();
      sendSmsToAllInfo("");//+makeGyroData());
      
      //ModuleState = true;
      }
  //handle GPS
  else if (sub_message.indexOf("+GPSRD") >= 0) {
      isEnable=0;
      String gdata = sub_message.substring(7);
      buildtrame(gdata);
      isEnable=1; 
      /*if (isAlarm) {
        double dst = CalcDistance(alflat, alflon, flat, flon );
        if (dst > 5) {
          Serial.print(dst);
          Serial.println(" m DANGER !!!");
          //sendSmsToAll("DANGER !!!!");
        }
      }*/
    }
  //manage messages from mqtt
  else if (sub_message.indexOf("getlist")>-1) {
    String val=JSONlistPeriph();
    connectGSM("AT+MQTTPUB=\""+mname+"\",\""+val+"\",0,0,0","",false);  
    }
  else if (sub_message.indexOf("start")>-1) {
    String val = sub_message;
    Serial.println(val);
    sendSocketCommand(val);
    }
  else if (sub_message.indexOf("stop")>-1) {
    String val = sub_message;
    sendSocketCommand(val);
    }
  else if (sub_message.indexOf("listen@")>-1) {
    String val = sub_message;
    //Serial.println("listen@");
    String topic = getValue(val, ',', 3);
    String service = getValue(topic, '@', 1);
    //Serial.println(topic);
    //sendSocketCommand(val);
    if(service.indexOf("globe")>-1){
      Serial.println(service);
      //connectGSM("AT+MQTTPUB=\""+mname+"\",\""+makeGpsDataJson()+"\",0,0,0","",false); 
      isListenGPS=1;
      }
    else if(service.indexOf("compass")>-1){
      Serial.println(service);
      //connectGSM("AT+MQTTPUB=\""+mname+"\",\""+makeMpuDataJson()+"\",0,0,0","",false); 
      isListenMPU=1;
      }
      
    
    }
  else if (sub_message.indexOf("close@")>-1) {
    String val = sub_message;
    String topic = getValue(val, ',', 3);
    String service = getValue(topic, '@', 1);
    //sendSocketCommand(val);
    if(service.indexOf("globe")>-1){
      Serial.println(service);
      isListenGPS=0;
      }
    else if(service.indexOf("compass")>-1){
      Serial.println(service);
      isListenMPU=0;
      }
    } 
  /*else if (sub_message !="" ){
    Serial.println(sub_message);  
    TelnetStream.println(sub_message);
  }*/
  // manage ping
  unsigned long t = millis();
  if((t - last_10sec) > 10 * 1000) {
        counter++;
        bool ping = 1;
        int i = bwebsocket.connectedClients(ping);
        //USE_SERIAL.printf("%d Connected websocket clients ping: %d\n", i, ping);
        last_10sec = millis();
        for(int z=0;z<i;z++){
          Serial.print(z);
          Serial.print("->");
          Serial.print(bwebsocket.clientIsConnected(z));
          Serial.print("->");
          Serial.println(lstPeriph[z]);
        }
     }
  
  if((t - last_1sec) > 5 * 1000) {
    last_1sec = millis();
    sendMqttMessage();
    }
  }
  #endif
}

void sendMqttMessage(){
  String msg="{'lsttable':[";
  int flag=0;
  if(isListenMPU){
    msg += makeMpuDataJson();
    flag=1;
  }
  if(isListenGPS){
    msg += makeGpsDataJson();
    flag=1;
  }
  if(flag){
    msg = msg.substring(0, msg.length() - 1);
    msg +="]}";
    TelnetStream.println(msg);
    connectGSM("AT+MQTTPUB=\""+mname+"\",\""+msg+"\",0,0,0","",false); 
  }
  
}
