/* todo
arret avant 4128

*/

const int buttonPin = 2;     // the number of the pushbutton pin
volatile int buttonState = 0;

unsigned long old_time;
volatile unsigned long new_time;

volatile int isInitSeq=1;
volatile int setmus=0;

int affwaitslave=0;
int affwaitsreset=0;
int affreadmessage=0;

volatile int waitesclave=0;
volatile int runread=0;

volatile int pos=0;
int valread=0;
volatile int vread=0;
volatile unsigned long  count = 0;
unsigned long prior_count=0;

uint8_t bitMask = 0x01;
int  numbit=0;
uint8_t r = 0;
uint8_t message[133];

void pin_ISR() {
  

  //reset
  if(isInitSeq){
    new_time = micros();
    old_time = micros();
    waitesclave=1;
    isInitSeq=0;
    }
  else if(waitesclave){
    waitesclave=0;
    runread=1;
    new_time = micros();
    setmus=new_time - old_time;
    old_time = new_time;
    }
  else if(runread){
    count=count+1;
    //new_time = micros();
    //buttonState = digitalRead(buttonPin);
  }
}

void setup() {
  // put your setup code here, to run once:
    // initialize the pushbutton pin as an input:
  Serial.begin(115200);


  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  count=0;
  attachInterrupt(digitalPinToInterrupt(2), pin_ISR, FALLING);


}

void reset(){
      runread=0;
      isInitSeq=1;
      waitesclave=0;
      affwaitsreset=0;
      affwaitslave=0;
      bitMask = 0x01;
      r=0;
      numbit=0;
      prior_count=0;
      count=0;
      new_time=0;
      pos=0;
}

void loop() {
  //for reset 
  if(isInitSeq && !affwaitsreset){
    affwaitsreset=1;

    delayMicroseconds(3);
    digitalWrite(8, HIGH); // sets the digital pin 13 on
    delayMicroseconds(1);
    digitalWrite(8, LOW);
    //Serial.println("reset");

  }

  if(waitesclave && !affwaitslave){
    affwaitslave=1;

    delayMicroseconds(3);
    digitalWrite(8, HIGH); // sets the digital pin 13 on
    delayMicroseconds(1);
    digitalWrite(8, LOW);
    //Serial.println("waitesclave");

  }


  unsigned long new_count = count;
  unsigned long new_timeC = new_time;

  if(runread && new_count != prior_count){

    prior_count = new_count;

    if(new_count == 1056){



      for(int ii=4;ii<132;ii++){
        //Serial.print(message[ii],HEX);
        Serial.write(message[ii]);
        //Serial.print(" ");
      }
      Serial.println("");
      reset();

    }

    else {
        //delayMicroseconds(1);
        valread=digitalRead(buttonPin);
        //si 8eme bit
        if(numbit ==7){
          digitalWrite(8, HIGH); 
          if (valread) r |= bitMask;
          message[pos++]=r;
          bitMask = 0x01;
          r=0;
          delayMicroseconds(6);
          numbit=0;
          digitalWrite(8, LOW); 
        }
        //sinon
        else{
          //et valread bit
          digitalWrite(8, HIGH); 

          if (valread) {
            r |= bitMask;
            //delayMicroseconds(3);
            }
          else delayMicroseconds(6);
          bitMask <<= 1;
          numbit++;
          digitalWrite(8, LOW); 
          }

        
    }

  }





}
