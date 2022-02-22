// Your GPRS credentials (leave empty, if not needed)
const char apn[]      = "sl2sfr"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// Server details
// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
const char server[] = "51.68.125.223"; // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/api";         // resource path, for example: /post-data.php
const int  port = 3000;  

#define TINY_GSM_MODEM_M95
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(14, 16);

#include <TinyGsmClient.h>


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
