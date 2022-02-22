#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
int i=2341;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
  
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(10);
  //mySwitch.setProtocol(2);
  //mySwitch.setPulseLength(320);
  mySwitch.setRepeatTransmit(15);

}

void loop() {
  // put your main code here, to run repeatedly:
  mySwitch.send(i, 24);
  delay(100);
  mySwitch.send(i, 24);
  delay(100);
  mySwitch.send(i, 24);
  delay(100);
  mySwitch.send(i++, 24);
  delay(1000);

}
