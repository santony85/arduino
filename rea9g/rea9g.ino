#include <SoftwareSerial.h> // Library for using serial communication
SoftwareSerial SIM900(5, 4); // Pins 7, 8 are used as used as software serial pins

String incomingData;   // for storing incoming serial data
//String message = "";   // A String for storing the message
//int relay_pin = 2;    // Initialized a pin for relay module
char msg;
char call;
void setup()
{
  Serial.begin(115200); // baudrate for serial monitor
  SIM900.begin(115200); // baudrate for GSM shield
  

  Serial.println("GSM SIM900A BEGIN");
  Serial.println("Enter character for control option:");
  Serial.println("h : to disconnect a call");
  Serial.println("i : to receive a call");
  Serial.println("s : to send message");
  Serial.println("c : to make a call");  
  Serial.println("e : to redial");
  Serial.println("l : to read location");
  Serial.println();
  delay(100);

  
  /*SIM900.println("AT+GPS=1");
  delay(100);
  SIM900.println("AT+GPSRD=5");
  delay(5000);*/
  
  // set SMS mode to text mode
  SIM900.print("AT+CMGF=1\r");  
  delay(100);
  
  // set gsm module to tp show the output on serial out
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);
}

void loop()
{
  
  receive_message();


if (Serial.available()>0)
   switch(Serial.read())
  {
    case 's':
      SendMessage();
      break;
    case 'c':
      MakeCall();
      break;
    case 'h':
      HangupCall();
      break;
    case 'e':
      RedialCall();
      break;
    case 'i':
      ReceiveCall();
      break;
    case 'l':
      ReadLocation();
      break;
  }
       
}

void receive_message()
{
  if (SIM900.available() > 0)
  {
    incomingData = SIM900.readString(); // Get the data from the serial port.
    Serial.print(incomingData); 
    delay(10); 
  }
}

void SendMessage()
{
  SIM900.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  SIM900.println("AT+CMGS=\"+33659401246\"\r"); // Replace x with mobile number
  delay(1000);
  SIM900.println("sim900a sms");// The SMS text you want to send
  delay(100);
   SIM900.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void ReceiveMessage()
{
  SIM900.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(1000);
  if (SIM900.available()>0)
  {
    msg=SIM900.read();
    Serial.print(msg);
  }
}

void MakeCall()
{
  SIM900.println("ATD+33659401246;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end, replace your number here!!
  Serial.println("Calling  "); // print response over serial port
  delay(1000);
}


void HangupCall()
{
  SIM900.println("ATH");
  Serial.println("Hangup Call");
  delay(1000);
}

void ReceiveCall()
{
  SIM900.println("ATA");
  delay(1000);
  {
    call=SIM900.read();
    Serial.print(call);
  }
}

void RedialCall()
{
  SIM900.println("ATDL");
  Serial.println("Redialing");
  delay(1000);
}
void ReadLocation(){

  SIM900.println("AT+GPS=1");
  delay(1000);
  SIM900.println("AT+GPSRD=5");
  delay(1000);
  
  }
