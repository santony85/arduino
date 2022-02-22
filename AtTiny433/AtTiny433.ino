/**************************************************************
   Émetteur ATTiny85
   
   La valeur d'un potentiomètre branché à l'ATTiny est
   transmise par RF 433 MHz
   
   Instructions complètes:
   
   http://electroniqueamateur.blogspot.com/2018/02/communication-rf-433-mhz-entre-attiny85.html
****************************************************************/

#include <Manchester.h>

#define NUMERO_EMETTEUR 8 // choisir un nombre entre 0 et 254, unique pour chaque émetteur du réseau
#define TX_PIN  11  // broche reliée à l'émetteur 433 MHz
#define POT_PIN A1 // broche reliée au potentiomètre dont nous désirons transmettre l'état
#define LED_PIN 0 // broche reliée à la LED clignotante (facultative)

uint8_t LedOn = 1; //statut de la LED clignotante facultative

#define datalength 4 //nombre de bytes que contiendra le message (minimum 2)
uint8_t data[datalength]; // matrice qui contiendra le message à envoyer

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LedOn);
  man.setupTransmit(TX_PIN, MAN_2400);
}


void loop()
{

  int mesure = analogRead(POT_PIN);  //on prend la valeur du potentiomètre
  
  data[0] = datalength; // nombre de bytes du message envoyé

  data[1] = NUMERO_EMETTEUR;   // pour que le récepteur sache de qui provient le message
  data[2] = highByte(mesure);  // la mesure du potentiomètre (10 bits) est répartie
  data[3] = lowByte(mesure);   // sur deux bytes

  man.transmitArray(datalength, data);
  
  LedOn = !LedOn;
  digitalWrite(LED_PIN, LedOn);

  delay(1000);

}
