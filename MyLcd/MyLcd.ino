#include "LiquidCrystal.h"

#include <Servo.h>

unsigned char Re_buf[11],counter=0;
unsigned char sign=0;
float a[3],w[3],angle[3],T;

Servo servo1;
Servo servo2;


int mcnt=0;

// initialize the library by providing the nuber of pins to it
LiquidCrystal lcd(3,2,4,5,6,7);

int ledPin = 12;

void setup() {
  Serial.begin(115200);
  Serial3.begin(9600);
  //JY901.attach(Serial3);
  // put your setup code here, to run once:
  lcd.begin(16,2);

// set cursor position to start of first line on the LCD
lcd.setCursor(0,0);
//text to print
lcd.print("            ");
// set cusor position to start of next line
lcd.setCursor(0,1);
lcd.print("   DISPLAY");
analogWrite(ledPin, 5);
servo1.attach(9);
servo2.attach(10);
}

void loop() {
 /*JY901.receiveSerialData();
  Serial.print("Gyro:");
  Serial.print(JY901.getGyroX());
  Serial.print(" ");
  Serial.print(JY901.getGyroY());
  Serial.print(" ");
  Serial.print(JY901.getGyroZ());
  Serial.print("\n");
  // set cursor position to start of first line on the LCD
lcd.setCursor(0,0);
//text to print
lcd.print("   GYRO");
// set cusor position to start of next line
lcd.setCursor(0,1);
  lcd.print(JY901.getGyroX());
  lcd.print(" ");
  lcd.print(JY901.getGyroY());
  lcd.print(" ");
  lcd.print(JY901.getGyroZ());*/
getData();
    if(sign)
  {  
     sign=0;
     if(Re_buf[0]==0x55)      //检查帧头
     {  
  switch(Re_buf [1])
  {
  case 0x51:
    a[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*16;
    a[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*16;
    a[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*16;
    T = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;
    break;
  case 0x52:
    w[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*2000;
    w[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*2000;
    w[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*2000;
    T = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;
    break;
  case 0x53:
          angle[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*180;
    angle[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*180;
    angle[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*180;
    T = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;

                
                if(mcnt==5){
                mcnt=0;
                lcd.setCursor(0,0);
                lcd.print("angle:");
                lcd.setCursor(0,1);
                lcd.print(angle[0]);lcd.print("     ");
                lcd.print(angle[1]);lcd.print(" ");
                //lcd.print(angle[2]);lcd.print(" ");
                  servo1.write(90-angle[0]);
                }
                mcnt++;

                
                Serial.print("angle:");

                
                Serial.print(angle[0]);Serial.print(" ");
                Serial.print(angle[1]);Serial.print(" ");
                Serial.print(angle[2]);Serial.print(" ");


/*
                
                Serial.print("T:");
                Serial.println(T);*/
                break;
  } 
    }

  } 
  
}

void getData(){
  while (Serial3.available()) 
  {
    
    Re_buf[counter]=(unsigned char)Serial3.read();
    if(counter==0&&Re_buf[0]!=0x55) return;     
    counter++;    
    if(counter==11)             
    {    
       counter=0;              
       sign=1;
    }
  }
}
