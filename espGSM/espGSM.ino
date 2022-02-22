#include <SoftwareSerial.h>

#define rxPin 16
#define txPin 17

SoftwareSerial mySerial(rxPin,txPin); // RX, TX

void setup(){

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("Arduino serial initialized!");
  delay(10);

  mySerial.begin(115200);
  Serial.println("Software serial initialized!");
  delay(10);
}

void loop(){
  issueCommand("AT");
  readSerial();
  delay(500);


  while(true){
    readSerial();
  }
}

void issueCommand(char* msg){
  mySerial.println(msg);
  Serial.print(msg);
  delay(10);
}

void readSerial(){
  while (mySerial.available()){
    Serial.write(mySerial.read());
    delay(10);
  }
}
