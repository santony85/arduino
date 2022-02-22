#include <ESP8266WiFi.h>
int port = 4206;  //Port number
WiFiServer server(port);

const char *ssid = "Livebox-4296";  
const char *password = "Santony85";  

void setup() {
  initHardware();
  setupWiFi();
  // Wait for connection  
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  server.begin();
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    if(client.connected())
      {
      Serial.println("Client Connected");
      }
    if (client.available() > 0) {
      char c = client.read();
      Serial.write(c);
    }
  }

}

void setupWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);
}

void initHardware()
{
  Serial.begin(115200);
}
