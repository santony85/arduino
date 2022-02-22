int rx0 = 4;    // pushbutton connected to digital pin 7
int rx1 = 5;    // pushbutton connected to digital pin 7
int vrx0 = 0;
int vrx1 = 0;
int cnt=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(rx0, INPUT);    // sets the digital pin 7 as input
  pinMode(rx1, INPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
    vrx0 = digitalRead(rx0);   // read the input pin
    vrx1 = digitalRead(rx1);   // read the input pin
    int msum = (vrx0*10)+vrx1;
    if(msum>0){
      Serial.print(cnt++);
      Serial.print("->");
      Serial.println(msum);
      if(msum==1)Serial.println("left");
      else if(msum==11)Serial.println("right");
      else if(msum==10)Serial.println("lumiere");
      delay(1000);
    }

}
