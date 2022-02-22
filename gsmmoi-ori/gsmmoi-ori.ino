#include <SoftwareSerial.h>

//GSM
SoftwareSerial serial_gsm(2, 3);
const int gsm_pwk = 7;

void setup()
{
  Serial.begin(9600);

  //GSM
  pinMode(gsm_pwk, OUTPUT);

  serial_gsm.begin(9600);
  //Allumage du module GSM
  digitalWrite(gsm_pwk, HIGH);
  delay(1100);
  digitalWrite(gsm_pwk, LOW);
}

void loop()
{
  debug_serial_gsm();
}



void debug_serial_gsm() {

  boolean boucle = true;
  while (boucle)
  {
    while (serial_gsm.available() > 0)
    {
      Serial.write(serial_gsm.read());
    }

    while (Serial.available() > 0)
    {
      serial_gsm.write(Serial.read());
    }
  }
}
