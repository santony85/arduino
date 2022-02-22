int cnt=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(15000);
}

void loop() {
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    delay(15000);
    Serial.println(9);
  }
  // put your main code here, to run repeatedly:
  if(cnt <15){
     cnt++;
     Serial.println(1);
     delay(250); 
  }


}
