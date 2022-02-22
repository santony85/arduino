#include <esp_camera.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#include <base64.h>
#include <WebSocketsClient.h>

String MQTT_PUBLISH_Monitor="";
String MQTT_PUBLISH_Command="";
String myMAC="";

unsigned long lastMsg = 0;
int count = 0;
 WiFiMulti WiFiMulti;
 WebSocketsClient webSocket;
 #define USE_SERIAL Serial
 String cap ="video-camera";
 
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
WiFiClient client;


const char* BROKER = "192.168.4.1";
const uint16_t BROKER_PORT = 1883;


String getMacAddress() {
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  String text =(char*)payload;
	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      if(text.indexOf("connected")>=0){
        USE_SERIAL.println("cnt Ok");
        webSocket.sendTXT("cap@"+cap);//send service
      }
      else if(text.indexOf("start")>=0){
        USE_SERIAL.println("Start Ok");
        //next();
        isStream=1;
      }
      else if(text.indexOf("stop")>=0){
        USE_SERIAL.println("Stop Ok");
        isStream=0;
      }
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			//hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}


void setup() {
  USE_SERIAL.begin(115200);
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
  USE_SERIAL.println("Start");

  WiFiMulti.addAP("ESP32-BOAT", "Santony85");
	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}
  
  myMAC = String(WiFi.macAddress());
  myMAC.replace(":","");
  MQTT_PUBLISH_Monitor="camera_"+myMAC;
	// server address, port and URL
	webSocket.begin("192.168.4.1", 4206, MQTT_PUBLISH_Monitor);
	webSocket.onEvent(webSocketEvent);
	webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);
  //setup_wifi();
  if (!MDNS.begin(MQTT_PUBLISH_Monitor.c_str())) {             // Start the mDNS responder for esp8266.local
    USE_SERIAL.println("Error setting up MDNS responder!");
  }
  USE_SERIAL.println("mDNS responder started");

  
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

  webSocket.sendBIN(fb->buf,fb->len);
  esp_camera_fb_return(fb);
}

void loop() {
  webSocket.loop();
  if(isStream){
    MQTT_picture();
    //delay(100);
    count++;
    if(count > 150){
       count=0;
      isStream=0; 
      }    
    }
}
