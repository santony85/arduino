#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver input on interrupt 0 (D2)
  pinMode(13, OUTPUT); // D13 as output- Optional
  Serial.print("Ready ");
}

void loop() {

  if (mySwitch.available()) {
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("Bit ");
    Serial.print(" // ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );
    digitalWrite(13, HIGH); // Toggle the onboard LED  if serial is available - Optional

    delay(1);

    digitalWrite(13, LOW);
    mySwitch.resetAvailable();
  }
}
