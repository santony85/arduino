int rx0 = 18;    // D7 pushbutton connected to digital pin 7
int rx1 = 5;    // D6 pushbutton connected to digital pin 7
int vrx0 = 0;
int vrx1 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(rx0, INPUT);    // sets the digital pin 7 as input
  pinMode(rx1, INPUT); 

}

void loop() {
  // put your main code here, to run repeatedly:
  vrx0 = digitalRead(rx0);
  vrx1 = digitalRead(rx1);
  int msum = (vrx0*10)+vrx1;
  if(msum>0){
      Serial.println(msum);
   }

}
