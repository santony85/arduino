
#include <WiFi.h>
#include <HTTPClient.h>

//Your IP address or domain name with URL path
const char* serverNameL = "http://192.168.4.1/l";
const char* serverNameR = "http://192.168.4.1/r";
const char* serverNameM = "http://192.168.4.1/m";

const char* ssid     = "RetroAP8266";
const char* password = "Santony85";

int pinX = 34; 
int pinY = 35; 

int valx = 0; 
int valy = 0;
int isAction=0;
int previousMillis=0, interval=2000;

IPAddress local_IP(192,168,4,3);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);           //  setup serial
  WiFi.begin(ssid, password);
  WiFi.config(local_IP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  valx = analogRead(pinX);  
  //delay(100);
  valy = analogRead(pinY);  
  if(isAction==0){
    if(valx < 5 || valx > 4090){
      if(valx <5){
        isAction=1;
        previousMillis=currentMillis;
        Serial.println("right"); 
        Serial.println(httpGETRequest(serverNameR));

      }
      if(valx > 4090){
        Serial.println("left");
        previousMillis=currentMillis;
        isAction=1;
        Serial.println(httpGETRequest(serverNameL));
       
      }
    }
    if(valy < 5 || valy > 4090){
      if(valy <5){
        Serial.println("up"); 
        isAction=1;
        previousMillis=currentMillis;
        Serial.println(httpGETRequest(serverNameM));
      }
      if(valy > 4090){
        Serial.println("down"); 
        isAction=1;
        previousMillis=currentMillis;
      }
    }
  }
  else{
    //Serial.println(currentMillis - previousMillis);
    if (currentMillis - previousMillis > interval) {
      isAction=0;
      }
  }


}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(8000);
  http.begin(serverName);
  int httpResponseCode = http.GET();
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    isAction=0;
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    isAction=0;
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
