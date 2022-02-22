
int tempVal;    // temperature sensor raw readings
float volts;    // variable for storing voltage 
float temp;     // actual temperature variable
float vmin = 2.0;
float vmax = 18.0;
int   vnum = 1;
float temptemp;


int Buzzer = 2; //for ESP32 Microcontroller

#include <EEPROM.h>
#include <Arduino.h>
#include <WiFi.h>         //Including wifi.h library it will take care of all wifi related task
#include <HTTPClient.h>   //Including HTTPClient.h library to use all api
#include <AsyncTCP.h>
#include "form2.h"
#include <ESPAsyncWebServer.h>
#include <driver/adc.h>
AsyncWebServer server(80);

#include <OneWire.h>
#include <DallasTemperature.h>
// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


const char* ssid = "iPhone de antony";             //Add your WiFi ssid
const char* password =  "Santony85";    //Add your WiFi password

String apiKey = "926315";              //Add your Token number that bot has sent you on signal messenger
String phone_number = "+33659401246"; //Add your signal app registered phone number (same number that bot send you in url)

String vname = "Cuve ";

String url;   
int address=0;
int alarmeencours=0;

#define EEPROM_SIZE 1024

String myIp;


const char* PARAM_1 = "vmin";
const char* PARAM_2 = "vmax";
const char* PARAM_3 = "vnum";
const char* PARAM_4 = "vname";
const String inputParam1 = "vmin";
const String inputParam2 = "vmax";
const String inputParam3 = "vnum";
const String inputParam4 = "vname";
//url String will be used to store the final generated URL

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readTemperature() {
    return String(temp);
}

String IpAddress2String(const IPAddress& ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}

void setup()
{

  // start the serial port at 9600 baud
  Serial.begin(115200);
  pinMode (Buzzer, OUTPUT);

  EEPROM.begin(EEPROM_SIZE);

  float readParam;
 // VMIN
  readParam=EEPROM.readFloat(address);
  Serial.print("Read param = ");
  Serial.println(readParam);
  if(String(readParam) == "0.00" || String(readParam) == "nan"){
    Serial.println("Res null vmin");
    EEPROM.writeFloat(address, vmin);//EEPROM.put(address, param);
    EEPROM.commit();
  }
  else vmin = readParam;
  address += sizeof(vmin);
  
  //VMAX
  readParam=EEPROM.readFloat(address);
  Serial.print("Read param = ");
  Serial.println(readParam);
  if(String(readParam) == "0.00" || String(readParam) == "nan"){
    Serial.println("Res null");
    EEPROM.writeFloat(address, vmax);//EEPROM.put(address, param);
    EEPROM.commit();
  }
  else vmax = readParam;
  address += sizeof(vmax);
  
  //VNUM
  int readNum=0;
  readNum=EEPROM.read(address);
  Serial.print("Read param = ");
  Serial.println(readNum);
  if(readNum == 255){
    Serial.println("Res null");
    EEPROM.write(address, vnum);//EEPROM.put(address, param);
    EEPROM.commit();
  }
  else vnum = readNum;
  address += sizeof(vnum);

  //VNAME
  String mname = EEPROM.readString(address);
  Serial.print("Read param = ");
  Serial.println(mname);
  if(mname==""){
    EEPROM.writeString(address, vname+" "+vnum);//EEPROM.put(address, param);
    EEPROM.commit();
    }
  else vname = mname;
  //EEPROM.writeString(address, sentence);

 WiFi.begin(ssid, password);              // Try to connect with the given SSID and PSS
 WiFi.mode(WIFI_STA);
 
    /*IPAddress ip(192,168,1,100+vnum);
    IPAddress gateway(192,168,1,1);
    IPAddress subnet(255,255,255,0);
    IPAddress dns(8, 8, 8, 8); // Google DNS
    WiFi.config(ip, gateway, subnet,dns);*/ 
    
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {  // Wait until WiFi is connected
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to the WiFi network"); // Print wifi connect message
  Serial.println();
  Serial.print("IP Address: ");
  
  myIp=IpAddress2String(WiFi.localIP());
  
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String res = index_html;
    res.replace("%VMIN%",String(vmin));
    res.replace("%VMAX%",String(vmax));
    res.replace("%VNUM%",String(vnum));
    res.replace("%VNAME%",String(vname));
    res.replace("%MYIP%",String(myIp));
    request->send_P(200, "text/html", res.c_str());
  });

  // Send web page with input fields to client
  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemperature().c_str());
  });

  // Send web page with input fields to client
  server.on("/alarm", HTTP_GET, [](AsyncWebServerRequest *request){
    String alarm = String(alarmeencours);
    request->send_P(200, "text/plain", alarm.c_str());
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1,inputMessage2,inputMessage3,inputMessage4;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->getParam(PARAM_1)->value() != "") {
      inputMessage1 = request->getParam(PARAM_1)->value();
    } else {
      inputMessage1 = "none";
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    if (request->getParam(PARAM_2)->value() != "") {
      inputMessage2 = request->getParam(PARAM_2)->value();
    } else {
      inputMessage2 = "none";
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    if (request->getParam(PARAM_3)->value() != "") {
      inputMessage3 = request->getParam(PARAM_3)->value();
    } else {
      inputMessage3 = "none";
    }
    
    if (request->getParam(PARAM_4)->value() != "") {
      inputMessage4 = request->getParam(PARAM_4)->value();
    } else {
      inputMessage4 = "none";
    }
    
    Serial.println(inputParam1 + ": " + inputMessage1);
    Serial.println(inputParam2 + ": " + inputMessage2);
    Serial.println(inputParam3 + ": " + inputMessage3);
    Serial.println(inputParam4 + ": " + inputMessage4);
    
    vmin = inputMessage1.toFloat();
    vmax = inputMessage2.toFloat();
    vnum = inputMessage3.toInt();
    vname = inputMessage4;

    int address = 0;
    EEPROM.writeFloat(address, vmin);//EEPROM.put(address, param);
    EEPROM.commit();
    address += sizeof(vmin);
    EEPROM.writeFloat(address, vmax);//EEPROM.put(address, param);
    EEPROM.commit();
    address += sizeof(vmax);
    EEPROM.write(address, vnum);//EEPROM.put(address, param);
    EEPROM.commit();
    address += sizeof(vnum);
    EEPROM.writeString(address, vname);//EEPROM.put(address, param);
    EEPROM.commit();    
    request->send(200, "text/html", "<html><head><meta http-equiv='refresh' content='0; URL=/'></head><body></body></html>"); 
});

  server.onNotFound(notFound);
  server.begin();


 /* pinMode(tempPin,INPUT);
  adcAttachPin(tempPin);
  analogReadResolution(11);
  analogSetAttenuation(ADC_6db);*/
  sensors.begin();
  
  //use message_to_whatsapp function to send your own message
  
  message_to_whatsapp("Restart ");  // you send your own message just change "hello from TechTOnions" to your message.
}


void loop()
{

 /*tempVal = analogRead(tempPin);
 //Serial.println(tempVal);
 volts = tempVal/1023.0;             // normalize by the maximum temperature raw reading range
 temp = (volts - 0.5) * 100 ;         //calculate temperature celsius from voltage as per the equation found on the sensor spec sheet.
 /*Serial.print(" Temperature is:   "); // print out the following string to the serial monitor
 Serial.print(temp);                  // in the same line print the temperature
 Serial.println (" degrees C");*/       // still in the same line print degrees C, then go to next line.

  sensors.requestTemperatures(); 
  float temp = sensors.getTempCByIndex(0);
 
 if(temp < vmin || temp > vmax){
    if(alarmeencours==0){
      //ouverture relais
      alarmeencours=1;
      String mess="Alarme Temperature: ";
      mess += temp;
      mess += " ° ";
      message_to_whatsapp(mess);
      temptemp = temp;
      }
    digitalWrite (Buzzer, HIGH); //turn buzzer on
    delay(200);
    digitalWrite (Buzzer, LOW);  //turn buzzer off
    delay(200);
    }
 else {
  if(alarmeencours==1){
      alarmeencours=0;
      String mess="Fin alarme Temperature cuve ";
      message_to_whatsapp(mess);
    }
  delay(1000);                         // wait for 1 second or 1000 milliseconds before taking the next reading. 
  }
 }


 /////// MESSAGERIE
 void  message_to_whatsapp(String message)       // user define function to send meassage to WhatsApp app
{
  message += vname;
  //adding all number, your api key, your message into one complete url
  url = "https://api.callmebot.com/whatsapp.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(message);
  Serial.println(url);
  postData(); // calling postData to run the above-generated url once so that you will receive a message.
}

void postData()     //userDefine function used to call api(POST data)
{
  int httpCode;     // variable used to get the responce http code after calling api
  HTTPClient http;  // Declare object of class HTTPClient
  http.begin(url);  // begin the HTTPClient object with generated url
  httpCode = http.POST(url); // Finaly Post the URL with this function and it will store the http code
  if (httpCode == 200)      // Check if the responce http code is 200
  {
    Serial.println("Sent ok."); // print message sent ok message
  }
  else                      // if response HTTP code is not 200 it means there is some error.
  {
    Serial.println("Error."); // print error message.
  }
  http.end();          // After calling API end the HTTP client object.
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
