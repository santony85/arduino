/**************************************
L'Arduino reçoit les données envoyées par l'ATTiny 
et les affiche dans le moniteur série.
Instructions complètes:
http://electroniqueamateur.blogspot.com/2018/02/communication-rf-433-mhz-entre-attiny85.html
***************************************/

#include "Manchester.h"

#define RX_PIN 4  // là où est branché le récepteur 433 MHz
#define LED_PIN 13

uint8_t LedOn = 1;
#define BUFFER_SIZE 22
uint8_t buffer[BUFFER_SIZE];

void setup() 
{
  pinMode(LED_PIN, OUTPUT);  
  digitalWrite(LED_PIN, LedOn);
  Serial.begin(9600);
  Serial.print("Message en provenance de l'emmeteur ");
  man.setupReceive(RX_PIN, MAN_2400);
  man.beginReceiveArray(BUFFER_SIZE, buffer);
}

void loop() 
{
  if (man.receiveComplete()) 
  {
    uint8_t receivedSize = 0;

    receivedSize = buffer[0];  // la taille du message se trouve dans le premier byte

    int valeur = (buffer[2] << 8) + buffer[3];  // on combine les deux bytes pour former la valeur analogique mesurée

    Serial.print("Message en provenance de l'emmeteur ");
    Serial.print(buffer[1]); 
    Serial.print(" : ");
    Serial.println(valeur);

    man.beginReceiveArray(BUFFER_SIZE, buffer);
    LedOn = !LedOn;
    digitalWrite(LED_PIN, LedOn);
  }
}