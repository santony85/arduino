#include <esp_camera.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>

#include <base64.h>

const char* ssid = "ESP32-BOAT";
const char* password = "Santony85";

const char* mqtt_server = "192.168.4.1";
const unsigned int mqtt_port = 1883;
#define MQTT_USER               ""             
#define MQTT_PASSWORD           ""        
String MQTT_PUBLISH_Monitor="";
String MQTT_PUBLISH_Command="";
String myMAC="";

unsigned long lastMsg = 0;
int count = 0;
 
// ------ OV2640相機設定 ------------
  #define CAMERA_MODEL_AI_THINKER
  #define PWDN_GPIO_NUM     32
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25  //Flash LED - Flash Light is On if SD card is present
  #define SIOC_GPIO_NUM     23

  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM       5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17

 #define VSYNC_GPIO_NUM    22
 #define HREF_GPIO_NUM     26
 #define PCLK_GPIO_NUM     21

int isStream=0;
 
char clientId[50];
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;

//PubSubClient mqttClient(mqtt_server, mqtt_port, callback, wifiClient);
//PubSubClient mqttClient(wifiClient);

#include "TinyMqtt.h" 

const char* BROKER = "192.168.4.1";
const uint16_t BROKER_PORT = 1883;

static MqttClient client;

int onPublishA(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t /* length */)
{ 
  String spl = String(payload);
  Serial.println(spl);  
  if(topic == std::string(MQTT_PUBLISH_Command.c_str()) && spl.indexOf("connected") >= 0){
    Serial.println("Connected  OK");
    }
  else if(topic == std::string(MQTT_PUBLISH_Command.c_str()) && spl.indexOf("start") >= 0){
    //client.unsubscribe(MQTT_PUBLISH_Monitor.c_str());
    isStream=1;
    Serial.println("Start OK");
    next();
    }
  else if(topic == std::string(MQTT_PUBLISH_Command.c_str()) && spl.indexOf("stop") >= 0){
    //client.subscribe(MQTT_PUBLISH_Monitor.c_str());
    isStream=0;
    Serial.println("Stop OK");
    }
  else if(topic == std::string(MQTT_PUBLISH_Command.c_str()) && spl.indexOf("nextf") >= 0){
    if(isStream==1)next();
    Serial.println("next");
    }  
  //else Serial << "--> client A received " << topic.c_str() << ", " << payload  << endl; 

  }



String getMacAddress() {
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}


void setup_wifi() {
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi Connected.  IP Address: ");
  Serial.println(WiFi.localIP());
  myMAC = getMacAddress();
  MQTT_PUBLISH_Monitor = "camera_"+myMAC;
  MQTT_PUBLISH_Command = "command_camera_"+myMAC;
   
}

void MQTT_picture() {
  //camera_fb_t * fb;    // camera frame buffer.
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    delay(100);
    Serial.println("Camera capture failed, Reset");
    ESP.restart();
  }
  
  Serial.println(fb->len);
  /*String base64data = base64::encode(fb->buf, fb->len);
  int sz = strlen(base64data.c_str());
  Serial.println(sz);*/
  
  //client.unsubscribe(MQTT_PUBLISH_Monitor.c_str());
  //client.publish(MQTT_PUBLISH_Monitor.c_str(),base64data.c_str());
  client.publish(MQTT_PUBLISH_Monitor.c_str(),(char*)fb->buf,fb->len);
  //Serial.println(base64data);
  //client.subscribe(MQTT_PUBLISH_Monitor.c_str());   
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.jpeg_quality = 10;  //10-63 lower number means higher quality
  config.fb_count = 2;
  config.frame_size = FRAMESIZE_QVGA ;// FRAMESIZE_ + UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
  esp_err_t err = esp_camera_init(&config);
  delay(500);
  setup_wifi();
  client.connect(BROKER, BROKER_PORT);  // Put here your broker ip / port
  client.publish("camera", MQTT_PUBLISH_Monitor);
  client.setCallback(onPublishA);
  client.subscribe(MQTT_PUBLISH_Monitor.c_str());
  client.subscribe(MQTT_PUBLISH_Command.c_str());
  
}


void next(){
  MQTT_picture();
  delay(100);
  count++;
  if(count > 15){
     count=0;
     isStream=0; 
    }
} 

void loop() {
  client.loop();
  

}
