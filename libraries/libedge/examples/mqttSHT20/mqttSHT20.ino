#include <SparkFunHTU21D.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <thingplug.h>

#define PIN_EXTRA_POWER   16
 
const char* ssid = "edgeiLAB";
const char* password = "iotiotiot";

void callbackFunction(char * data)
{
  float temp = 0;
  sscanf(data, "%f", &temp);
  printf("callbackFunction: temp=%f\n", temp);
} 

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
HTU21D sht20;

void setup()
{
  char addr[] = "mqtt.sktiot.com";
  char id[] = "onekick";
  char pw[] = "RkNteUZPemFZMnJyNVpzb3A3ZnFrTE42VEp4RkRTN2E4c3ozT3NGQWdmWVZtUlMvWGxCZEs5Z3pmR3hHVUllYQ==";
  char deviceId[] = "onekick_20180110_1";
  char devicePw[] = "123456";
  char container[]= "temperature";

  pinMode(PIN_EXTRA_POWER, OUTPUT);
  digitalWrite(PIN_EXTRA_POWER, LOW); // Node MCU 동작 시작
  
  sht20.begin();
  Serial.begin(115200);
  Wire.begin();
  WiFi.begin(ssid, password);
 
  // 연결 완료 까지 대기
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if(!mqttConnect(&mqttClient, addr, id, pw, deviceId)) {
    printf("1. mqtt connect failed\n");
    while(1);
  }

  if(!mqttCreateNode(&mqttClient, devicePw)) {
    printf("2. mqtt create node failed\n");
    while(1);
  }

  if(!mqttCreateRemoteCSE(&mqttClient)) {
    printf("3. mqtt create remote cse failed\n");
    while(1);
  }

  if(!mqttCreateContainer(&mqttClient, container)) {
    printf("4. mqtt create container failed\n");
    while(1);
  }

  if(!mqttCreateMgmtCmd(&mqttClient)) {
    printf("5. mqtt create mgmt cmd failed\n");
    while(1);
  }

  if(!mqttSubscribe(&mqttClient, callbackFunction)) {
    printf("6. mqtt subscribe notification failed\n");
    while(1);
  }
}

void loop()
{
  float temp = sht20.readTemperature();
  char strTemp[BUF_SIZE_SMALL];
  sprintf(strTemp, "%f", temp);
  mqttCreateContentInstance(&mqttClient, strTemp);
  delay(1000);
}


