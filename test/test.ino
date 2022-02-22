#include <Arduino.h>


//****************** --------- ************
//****************** Variables ************
//****************** --------- ************
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

const char* ssid     = "LudeloJudgeMaster";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "AardvarkBadgerHedgehog";     // The password of the Wi-Fi network
 uint64_t chipid; 
 WiFiMulti wifiMulti;
 HTTPClient http;
 char uuid[15]; //Create a Unique AP from MAC address

  String issid = "";
  String imdp = "";
  String ipartie = "";
  String iidp = "";
  bool isWatchingPartieRun=false; 
  bool isPlaying=false; 
  String joueurs[2];
  String idj1="";
  String idj2="";
  int pos=0;
  bool numjoueur=false;
  unsigned long mtime;
  
//****************** --------- ************
//****************** /Variables ************
//****************** --------- ************

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  connectwifi();
  }
  
//****************** --------- ************
//****************** Fonctions ************
//****************** --------- ************
void createUUID() {
  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid>>32);
  snprintf(uuid,15,"Ludelo-%04X",chip);
}

char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}

String ip2Str(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++) {
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}

String getValue(String payload,int pos){
  int vpos = payload.indexOf(";", pos);  
  if(pos == 0){
    return payload.substring(0,vpos);  
    }
  else {
    String strTmp="";
    Serial.println(pos);
    for(int i=0;i<pos;i++){
      payload.remove(0, vpos+1);
      vpos = payload.indexOf(";", i);
      strTmp = payload.substring(0,vpos);  
    }
    return strTmp;
  } 
}

void connectwifi(){
  createUUID();
  Serial.println(uuid);
  WiFi.begin(ssid, password);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  String payload; 
  String url= String("http://192.168.4.1:3000/getssid/")+String(uuid);
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
            if(httpCode == HTTP_CODE_OK) {
                payload = http.getString();
                
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
  http.end();
  WiFi.disconnect(false);
  int pos = payload.indexOf(";", 0);  
  issid = getValue(payload,0);
  imdp = getValue(payload,1);
  ipartie = getValue(payload,2);
  iidp = getValue(payload,3);
  Serial.println("");
  Serial.println(payload);
  Serial.println(issid);
  Serial.println(imdp);
  Serial.println(ipartie);
  Serial.println(iidp);
  newconnectwifi(string2char(issid),string2char(imdp));
}

void newconnectwifi(char* issid, char* ipassword){
    delay(5000);
    Serial.println("wait to ");
    WiFi.begin(issid, ipassword);             // Connect to the network
    Serial.println("Connecting to ");
    Serial.println(issid);
    while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
      delay(500);
      Serial.print('.');
    }
 
    Serial.println('\n');
    Serial.println("Connection established!");  
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  
  String payload; 
  String url= String("http://192.168.4.1:3000/ludeloconnect/")+String(uuid)+String("/")+String(ip2Str(WiFi.localIP())+String("/")+String(ipartie)+String("/")+String(iidp));
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
            if(httpCode == HTTP_CODE_OK) {
                payload = http.getString();
                
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error:\n");
        }
  Serial.println(payload); 
  isWatchingPartieRun=true;      
  http.end();
}

bool getPartieRunning(){
  String payload; 
  String url= String("http://192.168.4.1:3000/getpartierunning/")+String(ipartie)+String("/")+String(uuid);
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
            if(httpCode == HTTP_CODE_OK) {
                payload = http.getString();    
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error:\n");
        }  
  http.end();
  if(payload=="0")return false;
  else {
    idj1 = getValue(payload,0);
    idj2 = getValue(payload,1);
    joueurs[0]=idj1;
    joueurs[1]=idj2;
    Serial.println(idj1);
    Serial.println(idj2);
    isWatchingPartieRun=false;
    return true;
  }
}

bool sendData(String data, int pos){
  String payload; 
  String url= String("http://192.168.4.1:3000/setinfopartie/")+String(ipartie)+String("/")+String(uuid)+String("/")+String(data)+String("/")+String(joueurs[pos])+String("/")+String(pos);
  Serial.println(url);
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
            if(httpCode == HTTP_CODE_OK) {
                payload = http.getString();    
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error:\n");
        }
  http.end();
  if(payload=="1")return true; 
  else return false;
  
}

//****************** --------- ************
//****************** /Fonctions ************
//****************** --------- ************

void loop() { 
  
  if(isWatchingPartieRun){
    delay(500);
    if(getPartieRunning()){
     isWatchingPartieRun=false;
     isPlaying= true; 
     Serial.println("Et c'est parti");
    }
  }

  if(isPlaying){
    
    delay(2000);
    mtime = millis();
    Serial.println(mtime); 
    char mystr[40];
    sprintf(mystr,"%u",mtime);
    
    sendData(mystr, pos++);

    
    if(pos > 1)pos=0;
    Serial.println("Send Datas");
  }
   
}
