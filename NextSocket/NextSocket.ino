#include <Arduino.h>
#define DEGUB_ESP
#include <WiFi.h>
#include "esp_wifi.h"

#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}

int port = 4205;  //Port number
IPAddress server(51,68,125,223);

SocketIOclient socketIO;
int isConnected=0;

// Camera related
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
// Connection timeout;
#define CON_TIMEOUT   10*1000                     // milliseconds
#define TIME_TO_SLEEP (uint64_t)10*60*1000*1000   // microseconds

#define WIFI_SSID     "Livebox-4296"
#define WIFI_PASSWORD "Santony85"



#ifdef DEGUB_ESP
  #define DBG(x) Serial.println(x)
#else 
  #define DBG(...)
#endif

// Camera buffer, URL and picture name
camera_fb_t *fb = NULL;

String mName="ESP32-boat";

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            Serial.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
            Serial.printf("[IOc] Connected to url: %s\n", payload);
            isConnected=1;
            // join default namespace (no auto join in Socket.IO V3)
            socketIO.send(sIOtype_CONNECT, "/");
            sendSocket("adduser", mName);
            break;
        case sIOtype_EVENT:
            Serial.printf("[IOc] get event: %s\n", payload);
            break;
        case sIOtype_ACK:
            Serial.printf("[IOc] get ack: %u\n", length);
            //hexdump(payload, length);
            break;
        case sIOtype_ERROR:
            Serial.printf("[IOc] get error: %u\n", length);
            //hexdump(payload, length);
            break;
        case sIOtype_BINARY_EVENT:
            Serial.printf("[IOc] get binary: %u\n", length);
            //hexdump(payload, length);
            break;
        case sIOtype_BINARY_ACK:
            Serial.printf("[IOc] get binary ack: %u\n", length);
            //hexdump(payload, length);
            break;
    }
}

void sendSimple(){
  //socketIO.emit("adduser", mName);
  //socketIO.sendTXT("42[\"messageType\",{\"greeting\":\"hello\"}]");
}

void sendSocket(String room, String message){

  uint64_t now = millis();
  
  // creat JSON message for Socket.IO (event)
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();

  // add evnet name
  // Hint: socket.on('event_name', ....
  array.add(room);

  // add payload (parameters) for the event
  JsonObject param1 = array.createNestedObject();
  param1["data"] = message;

  // JSON to String (serializion)
  String output;
  serializeJson(doc, output);

  // Send event
  socketIO.sendEVENT(output);

  // Print JSON for debugging
  Serial.println(output);
   
}
void setup() {
  
  #ifdef DEGUB_ESP
    Serial.begin(115200);
    Serial.setDebugOutput(true);
  #endif
  // Initialize and configure camera
  //camera_init();
  connectWiFi();
  // server address, port and URL
  socketIO.begin("51.68.125.223", 3333, "/socket.io/?EIO=4");
  // event handler
  socketIO.onEvent(socketIOEvent);
  }

bool take_picture()
{
  DBG("Taking picture now");

  fb = esp_camera_fb_get();  
  if(!fb)
  {
    DBG("Camera capture failed");
    return false;
  }
  
  DBG("Camera capture success");

  return true;
}

void sendTcp()
{
  // Take picture
  take_picture();
  const char* pic_buf = (const char*)(fb->buf);
  size_t length = fb->len;
  //uint16_t packetIdPubTemp = mqttClient.publish( TOPIC_PIC, 0, false, pic_buf, length );
  DBG("buffer is " + String(length) + " bytes");
  delay(200);
  /*if( !packetIdPubTemp  )
  {
    DBG( "Sending Failed! err: " + String( packetIdPubTemp ) );
  }
  else
  {
    DBG("MQTT Publish succesful");
  }*/
  
}


void loop() {
  // put your main code here, to run repeatedly:
  socketIO.loop();
  if(isConnected){
    //sendSocket("event_name","100");
    //delay(2000); 
  }


}

bool camera_init()
{
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

  // IF USING A DIFFERENT BOARD, NEED DIFFERENT PINs
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
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

  //init with high specs to pre-allocate larger buffers
  config.frame_size   = FRAMESIZE_QVGA; // set picture size, FRAMESIZE_QQVGA = 160x120
  config.jpeg_quality = 10;            // quality of JPEG output. 0-63 lower means higher quality
  config.fb_count     = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.print("Camera init failed with error 0x%x");
    DBG(err);
    return false;
  }

  // Change extra settings if required
  //sensor_t * s = esp_camera_sensor_get();
  //s->set_vflip(s, 0);       //flip it back
  //s->set_brightness(s, 1);  //up the blightness just a bit
  //s->set_saturation(s, -2); //lower the saturation

  else
  {
    return true;
  }
  
}

void connectWiFi() {
  DBG("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED && millis() < CON_TIMEOUT){
    delay(500);
    Serial.print(".");
  }

  if( WiFi.status() != WL_CONNECTED ){
    DBG("Failed to connect to WiFi");
    delay( 600 );
    //deep_sleep();
  }

  DBG();
  DBG("IP address: ");
  DBG(WiFi.localIP());

}

