#include <SPI.h>
#include <IRremote.h>

byte address = 0x00;
int CS1= 10;

int volume=127;
int cnt=0;

int RECV_PIN = 8;
IRrecv irrecv(RECV_PIN); 
decode_results results; // decode_results class is defined in IRremote.h


/*int rx0 = 8;    // pushbutton connected to digital pin 7
int rx1 = 7;    // pushbutton connected to digital pin 7
int vrx0 = 0;
int vrx1 = 0;*/

void setup(){
  Serial.begin(115200);
  pinMode (CS1, OUTPUT);
  SPI.begin();
  //pinMode(rx0, INPUT);    // sets the digital pin 7 as input
  //pinMode(rx1, INPUT);  
  Serial.println("Start");
  digitalPotWrite1(0);
  irrecv.enableIRIn(); // Start the receiver 
  }

void loop(){
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    String res=String(results.value);
    
    if(res=="3772833823" && volume > 0){
      volume=volume-4;
      cnt--;
      Serial.print(cnt);
      Serial.print('-');
      digitalPotWrite1(volume);
      }    
    else if(res=="3772829743" && volume < 128){
      volume=volume+4;
      cnt++;
      Serial.print(cnt);
      Serial.print('-');
      digitalPotWrite1(volume);
      }
    
    irrecv.resume(); // Receive the next value 
  }
  delay (100); // small delay to prevent reading errors
  /*vrx0 = digitalRead(rx0);
  vrx1 = digitalRead(rx1);
  int msum = (vrx0*10)+vrx1;
  Serial.println(msum);
  if(msum > 0){
    Serial.println(msum);
    if(msum == 10 && volume <128){
      volume+=4;
      digitalPotWrite1(volume);
      }
    else if(msum == 11 && volume > 0){
      volume-=4;
      digitalPotWrite1(volume);
    }  
    delay(1000);
  }*/
  
}

int digitalPotWrite1(int value){
  Serial.println(value);
  digitalWrite(CS1, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(CS1, HIGH);
  //delay(1000);
  }
