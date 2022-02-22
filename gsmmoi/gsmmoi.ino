#include <GSM.h>

#define PINNUMBER ""

GSM gsmAccess;
GSM_SMS sms;

char c,Number[20],Msg[200];

void setup() 
{
  Serial.begin(9600);
  Serial.println("GSM Modem Lesson");
     
  boolean notConnected = true;
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY) notConnected=false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  
  Serial.println("GSM initialized");
  Serial.println("Enter 1 to send SMS\n");
}

void loop() 
{
  if(Serial.available())
  {
    if(Serial.read()=='1')
    {
      Serial.flush();
      Serial.print("Enter mobile number: ");
      readSerial(Number);
      Serial.println(Number);
      
      Serial.print("Enter message: ");
      readSerial(Msg);
      Serial.println(Msg);
      Serial.println();
      
      sms.beginSMS(Number);
      sms.print(Msg);
      sms.endSMS(); 
    }
  }
  
  if(sms.available())
  {
    Serial.print("Message received from: ");
    sms.remoteNumber(Number,20);
    Serial.println(Number);
    
    Serial.print("Message: ");
    while(c=sms.read()) Serial.print(c);    
    Serial.println("\n");
    sms.flush();            //delete SMS
  }
}

int readSerial(char result[])
{
  int i=0;
  while(Serial.available()==0) continue;
  delay(100);
  while(Serial.available()>0)
  {
    char inChar=Serial.read();
    result[i]=inChar;
    i++;
  }
  result[i]='\0';
}
