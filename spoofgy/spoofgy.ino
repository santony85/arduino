#include <SoftwareSerial.h>
SoftwareSerial ssMPU(2,3);
unsigned char Re_buf[11],counter=0;
unsigned char sign=0;
float a[3],w[3],angle[3],T,m[3];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ssMPU.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
serialEvent();

}
void serialEvent() {
  while (ssMPU.available()) {
    
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code 
    unsigned char tchar = ssMPU.read();
    Serial.print(tchar,HEX);
    Re_buf[counter]=tchar;
    if(counter==0&&Re_buf[0]!=0x55) return;      //第0号数据不是帧头              
    counter++;       
    if(counter==11)             //接收到11个数据
    {    
       counter=0;               //重新赋值，准备下一帧数据的接收 
       sign=1;
       Serial.println();
    }
      
  }
}
