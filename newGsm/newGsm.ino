//#include <SoftwareSerial.h>
//SoftwareSerial serial_gsm(17, 13);
const int gsm_pwk = 37;
const int MSTATUS = 36;
int stat = 0;
String resp = "";

#include "heltec.h"
#include "image.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}


void setup() {
  // put your setup code here, to run once:
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  logo();
  delay(1500);
  Serial.println("GSM modem starting");
  Serial.println(digitalRead(MSTATUS));
  pinMode(gsm_pwk, OUTPUT);
  pinMode(MSTATUS, INPUT);
  digitalWrite(gsm_pwk, HIGH);
  
  while (stat != 4095)
    {
        stat = analogRead(MSTATUS); // For an ADC with a 10 bit resolution 5V = 2^10-1
        Serial.println(stat);
    }

    // The modem is properly powered up, and we can now send AT commands
    Serial.println("GSM modem is powered-up!");
    digitalWrite(gsm_pwk, LOW);
    Serial1.begin(9600);
    Serial1.println( "AT" ); // Sending the 1st AT command
    
    // A mandatory 2-3 sec delay before expecting an answer according to QUECTEL's M95 documentation
    delay(3000);
    
    Serial1.println( "AT" ); // Sending the 2nd AT command

    while (Serial1.available())
    {
        char c = Serial1.read();
        resp += c;
    }

    resp[resp.length()-1] = '\0';
    Serial.println(resp); // The M95 modem should reply OK

  
}

void loop() {
//debug_serial_gsm();


}

void debug_serial_gsm() {

  boolean boucle = true;
  while (boucle)
  {
    /*while (serial_gsm.available() > 0)
    {
      Serial.write(serial_gsm.read());
    }*/

    while (Serial.available() > 0)
    {
      //serial_gsm.write(Serial.read());
    }
  }
}
