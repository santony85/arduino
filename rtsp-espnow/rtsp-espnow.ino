#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x02, 0xCF,0xBA,0xAC,0x6E,0xFE};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // send data
  int x = 10;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &x, sizeof(int));
     
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
