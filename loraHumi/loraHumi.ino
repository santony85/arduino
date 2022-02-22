#include "heltec.h"
#include "image.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

#include <SoftwareSerial.h>

//#include <TinyGPS.h>
//TinyGPS gps;

#include <TinyGPS++.h>
TinyGPSPlus gps;

SoftwareSerial ss(16, 17);

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

String lat="",lng="";
int poslatlng=0;



void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void setup() {

  
  //Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Informations :");
  Heltec.display->display();
  delay(1000);
  Serial.println("init ok");
  //ss.begin(9600, SERIAL_8N1, 16, 17);
  ss.begin(4800);
  
}
void loop() {

}