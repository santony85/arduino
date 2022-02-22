#include <PWM.h> //PWM librarey for controlling freq. of PWM signal
#include <LiquidCrystal.h>

#include "wavetable.h"
 
float seconds = 0;
int i = 0;
int num_readings = 0;



#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable


//const int rs = 14, en = 15, d4 = 4, d5 = 3, d6 = 6, d7 = 7; //Mention the pin number for LCD connection
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //Mention the pin number for LCD connection
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


int Encoder_OuputA  = 11;
int Encoder_OuputB  = 12;
int Encoder_Switch = 10;
int Previous_Output;
int multiplier = 1;

double angle = 0;
double increment = 0.2;


const int signal_pin = 9;  //9
const int Sine_pin = A0;
const int POT_pin = 13; 

       
int32_t frequency; //frequency to be set
int32_t lower_level_freq = 1; //Lowest possible freq value is 1Hz
int32_t upper_level_freq = 100000; //Maximum possible freq is 100KHz

static uint16_t state=0,counter=0;

int nbtemps=0;
int mode=0;
int setmode=0;

void setup()
{
  lcd.begin(16, 2); //Initialise 16*2 LCD

  lcd.print("Signal Generator"); //Intro Message line 1
  lcd.setCursor(0, 1);
  lcd.print("-CircuitDigest "); //Intro Message line 2
  delay(2000);
  lcd.clear();
  lcd.print("Freq:00000Hz");
  lcd.setCursor(0, 1);
  lcd.print("div: 1   Square");
  
  Serial.begin(9600); //Serial for debugging
  InitTimersSafe(); //Initialize timers without disturbing timer 0
  
  
//pin Mode declaration 
  pinMode (Encoder_OuputA, INPUT_PULLUP);
  pinMode (Encoder_OuputB, INPUT_PULLUP);
  pinMode (Encoder_Switch, INPUT_PULLUP);

  Previous_Output = digitalRead(Encoder_OuputA); //Read the inital value of Output A

 
  //startMozzi(CONTROL_RATE); // :)
  //aSin.setFreq(440); // set the frequency
}

void updateControl(){
  // put changing controls in here
}


int updateAudio(){
  return aSin.next(); // return an int signal centred around 0
}


void loop()
{



    delayMicroseconds(50); // Simulate doing somehing else as well.
    state=(state<<1) | digitalRead(Encoder_OuputA) | 0xe000;
    if (state==0xf000){
       state=0x0000;
       if(digitalRead(Encoder_OuputB)){
        counter++;
        frequency = frequency + multiplier;
        if(mode==0){
          pwmWriteHR(signal_pin, 32768); //Set duty cycle to 50% by default -> for 16-bit 65536/2 = 32768
          SetPinFrequencySafe(signal_pin, frequency);
          }
        else{
         aSin.setFreq((int)frequency); 
        }  
        lcd.setCursor(0, 0);
        lcd.print("Freq:     Hz");
        lcd.setCursor(5, 0);
        lcd.print(frequency);
        }
         
       else{
        counter--;
        frequency = frequency -   multiplier;
        if(mode==0){
          pwmWriteHR(signal_pin, 32768); //Set duty cycle to 50% by default -> for 16-bit 65536/2 = 32768
          SetPinFrequencySafe(signal_pin, frequency);
          }
        else{
         aSin.setFreq((int)frequency); 
        }
        lcd.setCursor(0, 0);
        lcd.print("Freq:     Hz");
        lcd.setCursor(5, 0);
        lcd.print(frequency);
        }
    }


    if (digitalRead(Encoder_Switch) == 0)
     {

      multiplier = multiplier * 10;
      if (multiplier>1000) multiplier=1;
      lcd.setCursor(0, 1);
      lcd.print("div:     ");
      lcd.setCursor(4, 1);
      lcd.print(multiplier);
      lcd.print(" ");
   
      delay(500);
      while(digitalRead(Encoder_Switch) == 0){
         nbtemps++;
         if(nbtemps > 4000)setmode=1;
        };
       }
     if(setmode==1){
       setmode=0;
       lcd.setCursor(9, 1);
       if(mode==0){
         mode=1;
         lcd.print("Sine   ");
         startMozzi(CONTROL_RATE); // :)
         aSin.setFreq((int)frequency); // set the frequency
         }
       else {
        mode=0;
        stopMozzi();
        lcd.print("Square");
        pwmWriteHR(signal_pin, 32768); //Set duty cycle to 50% by default -> for 16-bit 65536/2 = 32768
        SetPinFrequencySafe(signal_pin, 0);
       }
       nbtemps=0;
       
      }
      
      Previous_Output = digitalRead(Encoder_OuputA);  
      if(mode==1)audioHook(); // required here

}
