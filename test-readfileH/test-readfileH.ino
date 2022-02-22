#include "lines.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  // Debugging only

}

// Draw a XBM
void drawXbm(const uint8_t *xbm){
  for(int i=0;i<10;i++){
    //Serial.println(logo_bits);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(logo_bits);
  for(int i=0;i<40;i++){
    int a = logo_bits[i*3];
    int b = logo_bits[(i*3)+1];
    int c = logo_bits[(i*3)+2];

    Serial.print("led ");
    Serial.print(i);
    Serial.print(" (");
    Serial.print(a);
    Serial.print(",");
    Serial.print(b);
    Serial.print(",");
    Serial.print(c);
    Serial.println(")");
  }
}
