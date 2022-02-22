const int analogInPin1 = A0; 
static unsigned char adcval;
int ctr=0;
bool flag_tog = false;

void setup()
{
  Serial.begin(115200);
 
  pinMode(9, OUTPUT);

  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}

ISR(ADC_vect)
{
  adcval = ADCH;  // read 8 bit value from ADC
}
 
void loop()
{
  Serial.write(adcval);
 
  // Following code to generate ref signal at pin 13 @ 50HZ. You can connect A0 to see the waveform in PCScope.exe
  ctr++;
  if(ctr>117) //117=10.03ms
  {
   ctr=0;
   flag_tog = !flag_tog;
   digitalWrite(9, flag_tog);
  }
   
}
