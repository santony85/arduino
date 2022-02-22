
#define RXD2 16
#define TXD2 17

#define PWRKEY 21
#define STATUS 36
#define ADC_RES 1024

//#include <SoftwareSerial.h>
//SoftwareSerial ss(RXD2, TXD2);

int stat = 0;
String resp = "";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("bootup");
//  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  
    pinMode(PWRKEY, OUTPUT);
    pinMode(STATUS, INPUT);
    // Make sure the PWRKEY on the GSM2 Click board is at logic LOW
    digitalWrite(PWRKEY, LOW);
    delay(250); // This isn't really neccessary...

    // Put the PWRKEY of the GSM@ Click board at logic HIGH
    // and wait until STATUS is at logic HIGH
    digitalWrite(PWRKEY, HIGH);
    while (stat != 0)
      {
      stat = analogRead(STATUS); // For an ADC with a 10 bit resolution 5V = 2^10-1
      Serial.println(stat);
      }
        // The modem is properly powered up, and we can now send AT commands
    Serial.println("GSM modem is powered-up!");


    
    
    // A mandatory 2-3 sec delay before expecting an answer according to QUECTEL's M95 documentation
    delay(5000);
    digitalWrite(PWRKEY, LOW);
    Serial.println("Starting GSM2 UART...");
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    

}

void loop() {
    delay(5000);
    Serial2.println( "AT" ); // Sending the 2nd AT command
    resp="";
    while (Serial2.available())
    {
        char c = Serial2.read();
        resp += c;
    }

    resp[resp.length()-1] = '\0';
    Serial.println(resp); // The M95 modem should reply OK
  // put your main code here, to run repeatedly:
  /*while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
  while (Serial.available()) {
    Serial2.print(char(Serial.read()));
  }*/
}
