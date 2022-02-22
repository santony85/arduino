#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>


//#include <EEPROM.h>
#include "LittleFS.h"

#include "form3.h"

#define APSSID "raspicuve"
//#define APSSID "Livebox-4296"
#define APPSK  "Santony85"

ESP8266WebServer server(80);

#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

String apiKey = "926315";              //Add your Token number that bot has sent you on signal messenger
String phone_number = "+33650868740"; //Add your signal app registered phone number (same number that bot send you in url)

float temp;     // actual temperature variable
float vmin = 20;
float vmax = 26;
int   vnum = 9;
String vname = "Change Cuve";
float temptemp;



String url;   
int address=0;
int alarmeencours=0;
int relaison=0;
#define EEPROM_SIZE 1024

String myIp;

int Buzzer = 5;
int Relais = 13;//13

int isOnOff = 1;



const char* PARAM_1 = "vmin";
const char* PARAM_2 = "vmax";
const char* PARAM_3 = "vnum";
const char* PARAM_4 = "vname";
const String inputParam1 = "vmin";
const String inputParam2 = "vmax";
const String inputParam3 = "vnum";
const String inputParam4 = "vname";



String IpAddress2String(const IPAddress& ipAddress)
{
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
  server.on("/get/"+myIp,handleGet);
  server.on("/setonoff/"+myIp,handleOnOff);

  delay(1000);
  sensors.begin();

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

  
  /*EEPROM.begin(EEPROM_SIZE);
  int EEaddress = 0;
  int isInit  = EEPROM.read(EEaddress);
  Serial.println(isInit);
  if(isInit !=123 ){
    //Serial.println("init EEPROM");
    EEPROM.write(EEaddress,123);
    EEaddress=EEaddress+ 1;
    EEPROM.write(EEaddress,vnum);
    EEaddress=EEaddress+1;
    EEPROM.put(EEaddress,vmin);
    EEaddress = EEaddress + sizeof(float);
    EEPROM.put(EEaddress,vmax);
    EEaddress = EEaddress + sizeof(float);
    EEPROM.put(EEaddress,vname);
    EEPROM.commit(); 
    }
  else {
    EEaddress=1;
    vnum = EEPROM.read(EEaddress);
    //Serial.println(vnum);
    EEaddress=EEaddress+1;
    EEPROM.get(EEaddress,vmin);
    EEaddress = EEaddress + sizeof(float);
    EEPROM.get(EEaddress,vmax);
    EEaddress = EEaddress + sizeof(float);
    EEPROM.get(EEaddress,vname);
    }
   Serial.println("Start OK");
   Serial.println(vname);
   Serial.println(vmax);
   Serial.println(vmin);
   Serial.println(vnum);*/

   
   delay(3000);
  }

void(* resetFunc) (void) = 0;


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


String readTemperature() {
    return String(temp);
}

void handleOnOff(){
  Serial.println("/setonoff");
  isOnOff = !isOnOff;
  writeData(makeString());
  server.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; URL=/'></head><body></body></html>"); 
}

void handleGet(){
    Serial.println("/get");
    vname = server.arg(0);
    vmax  = server.arg(1).toFloat();
    vmin  = server.arg(2).toFloat();
    vnum  = server.arg(3).toInt();

    Serial.println(vname);
    Serial.println(vmax);
    Serial.println(vmin);
    Serial.println(vnum);

    writeData(makeString());

    /*int EEaddress = 0;
    EEPROM.write(EEaddress,123);
    EEaddress=EEaddress+ 1;
    EEPROM.write(EEaddress,vnum);
    EEaddress=EEaddress+1;
    EEPROM.put(EEaddress,vmin);
    EEaddress = EEaddress + sizeof(float);
    EEPROM.put(EEaddress,vmax);
    EEaddress = EEaddress + sizeof(float);
    EEPROM.put(EEaddress,vname);
    EEPROM.commit();*/ 

    server.send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; URL=/'></head><body></body></html>"); 

}


void handleTemp(){
    //Serial.println("/temp");
    String retval = readTemperature()+";"+String(alarmeencours)+";"+String(relaison)+";"+String(isOnOff)+";"+String(vname)+";"+String(vmax)+";"+String(vmin);
    server.send(200, "text/plain", retval.c_str());
  }

void handleRoot(){
    //Serial.println("/");
    String res = index_html;
    res.replace("%VMIN%",String(vmin));
    res.replace("%VMAX%",String(vmax));
    res.replace("%VNUM%",String(vnum));
    res.replace("%VNAME%",String(vname));
    res.replace("%MYIP%",String(myIp));
    res.replace("%VTEMP%",readTemperature());
    
    server.send(200, "text/html", res.c_str());
  }
  
void loop(){ 
  server.handleClient();
  sensors.requestTemperatures(); 
  temp = sensors.getTempCByIndex(0);
  Serial.println(temp);
  //if(temp == -127)resetFunc();  //call reset
  if(isOnOff){
  
  //***** ALARME *****
  if(temp > vmax){
    Serial.println("err 1 ");  
    Serial.println(vmax);  
    if(alarmeencours==0){
      alarmeencours=1;
      temptemp = temp;
      }
    digitalWrite (Buzzer, LOW); //turn buzzer on
    delay(100);
    digitalWrite (Buzzer, HIGH);  //turn buzzer off
    delay(100);
    }
  //***** RELAIS ****
  else if(temp > (vmin+1) && temp < vmax){
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
      /*String mess="Alarme Temperature trop basse : ";
      mess += temp;
      mess += " ° ";
      message_to_whatsapp(mess);
      temptemp = temp;*/
      } 
    /*digitalWrite (Buzzer, LOW); //turn buzzer on
    delay(100);
    digitalWrite (Buzzer, HIGH);  //turn buzzer off
    delay(100);*/

   } 
  else {
    digitalWrite(Relais,HIGH);
    relaison=0;
    if(alarmeencours==1){
      alarmeencours=0;
      //String mess="Fin alarme Temperature cuve ";
      //message_to_whatsapp(mess);
      }                        // wait for 1 second or 1000 milliseconds before taking the next reading. 
    } 
  } 
  else{
    //Serial.println("is off");
    alarmeencours=0;
    digitalWrite (Buzzer, HIGH);  //turn buzzer off
    delay(100);
    digitalWrite (Relais, HIGH);  //turn buzzer off
    delay(100);
    }
  delay(1000);
}

 /////// MESSAGERIE
 void  message_to_whatsapp(String message)       // user define function to send meassage to WhatsApp app
{
  message += vname;
  //adding all number, your api key, your message into one complete url
  url = "https://api.callmebot.com/whatsapp.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(message);
  //Serial.println(url);
  postData(); // calling postData to run the above-generated url once so that you will receive a message.
}

void postData()     //userDefine function used to call api(POST data)
{
  // variable used to get the responce http code after calling api
  //WiFiClient client;

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient http;  
  http.begin(*client,url);  
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
  http.end(); 
}

String urlencode(String str)  // Function used for encoding the url
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}
