/* 
This sketch reads info from a samsung remote..
and the address and data are extracted
*/

#include <Servo.h>

#define tsop_pin 13
#define led_pin 12

int val=180;

Servo myservo;

void setup()
{
  Serial.begin(9600);
  pinMode(tsop_pin,INPUT);
  pinMode(led_pin,OUTPUT);
  Serial.println("ok");
  myservo.attach(9);
  myservo.write(val); 
}

void loop()
  {

  //myservo.write(val++);                  // sets the servo position according to the scaled value
  //delay(1000); 

  boolean bin[32];                    //boolean consumes less space
  int addr=0, data=0;
  
  while(pulseIn(tsop_pin,LOW)<4000);  //wait for start bit

  for(int i=0; i<32; i++)
  {
    if(pulseIn(tsop_pin,HIGH)>1000)   //store binary values
      bin[i]=1;
    else
      bin[i]=0;
  }

  //Serial.print("Address = ");
  for(int i=0;i<8;i++)            //extract addr bits
  {
    //Serial.print(bin[i],BIN);
    if(bin[i]==1)
      addr+=1<<i;
  }
  //Serial.print("   ");
  //Serial.println(addr);
  
  
  //Serial.print("Data = ");
  for(int i=0;i<8;i++)            //extract data bits
  {
    //Serial.print(bin[i+16],BIN);
    if(bin[i+16]==1)
      data+=1<<i;
  }
  //Serial.print("   ");
  //Serial.println(data);
  //Serial.println();
  
  //delay(50);                      //some time to breathe ;)
  //digitalWrite(led_pin,LOW);      //turn off the led

  if(data==11){
    //Serial.println("up");
    if(val >= 10 && val <= 180){
      myservo.write(val++);
    }
    Serial.println(val);
  }
  else if(data==7){
    //Serial.println("down");
    Serial.println(val);
    if(val <= 180 ){
      myservo.write(val--);
    }
    
    
  }
delay(50);

}