/*
  Programme : Blink
  Auteur : PHMARDUINO
  Date : Le 22/01/2020
  Description : Fait clignoter une LED sur un ATtiny85 branchée sur PB4 (broche 3)
 */

#include <VirtualWire.h>

int btl = 1;    
int btr = 2;

// Initialisations
void setup() {
  pinMode(btl, INPUT);
  pinMode(btr, INPUT);
  vw_set_tx_pin(0);
  vw_setup(2000);
}

// Boucle infinie
void loop() {
  if(digitalRead(btl)){
    const char *msg = "SendL";  
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone  
  }
  else if(digitalRead(btr)){
    const char *msg = "SendR";  
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone  
  }

  delay(5000);
}
