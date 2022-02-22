#include <SoftwareSerial.h>
//#define ssGSM Serial2
SoftwareSerial ssGSM(35, 34);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");
  ssGSM.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0) {
    String msgIn = Serial.readString();
    ssGSM.println(msgIn);
  }
  if(ssGSM.available() > 0) {
    String msgIn = ssGSM.readString();
    Serial.println(msgIn);
  }

}
