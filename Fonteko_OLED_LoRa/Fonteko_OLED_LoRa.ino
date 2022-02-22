/*
  This is a simple example show the Heltec.LoRa sended data in OLED.

  The onboard OLED display is SSD1306 driver and I2C interface. In order to make the
  OLED correctly operation, you should output a high-low-high(1-0-1) signal by soft-
  ware to OLED's reset pin, the low-level signal at least 5ms.

  OLED pins to ESP32 GPIOs via this connecthin:
  OLED_SDA -- GPIO4
  OLED_SCL -- GPIO15
  OLED_RST -- GPIO16
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  https://heltec.org
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/

#include "heltec.h"
#include "images.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

unsigned int counter = 0;
String rssi = "RSSI --";
String packSize = "--";
String packet ;
String inSerial = "";
int currentTime;
int isAcc=0;
String myid="a3";

void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void setup() {
  //Serial.begin(115200); 
   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->display();
  delay(1000);
  Serial.println("init ok");
}

void LoRaData(){
  Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet); 
  Heltec.display->display();
  isAcc=0;

  Serial.println(packet);
  //si long == 16 >- qrcode -> decrement credit
  //else
  //si packet
  //LoRa.beginPacket();
  //LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
  //LoRa.print(myid);
  //LoRa.endPacket();

  //delay(2000);

}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
}

void setConfig(){
  //myid
  //nbinpark
  
}

void loop() {
//from raspberry  
  if (Serial.available() > 0) { 
    inSerial = Serial.readString();

    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, "Sending packet: ");
    Heltec.display->drawString(0, 11, inSerial);
    Heltec.display->display();

    LoRa.beginPacket();
    LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
    LoRa.print(inSerial);
    LoRa.endPacket();
    //isAcc=1;
    //currentTime=millis();//set timeout
    Serial.println("x");
  }
//*******************  
  else {
    int packetSize = LoRa.parsePacket();
    if (packetSize) { cbk(packetSize);  }
    delay(10);
  }

    
/*
  if(isAcc==0){
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, "Sending packet: ");
    Heltec.display->drawString(0, 11, "1200125011001222");
    Heltec.display->display();
    LoRa.beginPacket();
    LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
    LoRa.print("1200125011001222");
    LoRa.endPacket();
    isAcc=1;
    currentTime=millis();//set timeout
    }
  else{
    long duration = millis() - currentTime;
    if(duration < 4000){
      int packetSize = LoRa.parsePacket();
      if (packetSize) { cbk(packetSize);  }
      delay(10);
      }
    else isAcc=0;
  }  
*/
}
