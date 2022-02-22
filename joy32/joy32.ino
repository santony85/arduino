void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Début...");
}


void handleADC() {
 analogReadResolution(6);
 int x = analogRead(A0);
 int y = analogRead(A3);
 int s = analogRead(34);
 String adcValue = "X="+String(x)+"|Y="+String(y);
 Serial.println(adcValue);
 //server.send(200, "text/plane", adcValue); //Envoi des valeurs lues
}

void loop() {
  // put your main code here, to run repeatedly:
  handleADC();
  delay(100);

}
