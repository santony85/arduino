// Arduino NEC Protocol IR remote control decoder
 
 
// include LCD library code
#include <LiquidCrystal.h>
 
// LCD module connection (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);
 
char text[5];
boolean nec_ok = 0;
byte  i, nec_state = 0, command, inv_command;
unsigned int address;
unsigned long nec_code;
 
void setup() {
  Serial.begin(115200);
  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Address:0x0000");
  lcd.setCursor(0, 1);
  lcd.print("Com:0x00 In:0x00");
  // Timer1 module configuration
  TCCR1A = 0;
  TCCR1B = 0;                                    // Disable Timer1 module
  TCNT1  = 0;                                    // Set Timer1 preload value to 0 (reset)
  TIMSK1 = 1;                                    // enable Timer1 overflow interrupt
  attachInterrupt(0, remote_read, CHANGE);       // Enable external interrupt (INT0)
  Serial.println(F("Send RC5"));
}
 
void remote_read() {
unsigned int timer_value;
  if(nec_state != 0){
    timer_value = TCNT1;                         // Store Timer1 value
    TCNT1 = 0;                                   // Reset Timer1
  }
  switch(nec_state){
   case 0 :                                      // Start receiving IR data (we're at the beginning of 9ms pulse)
    TCNT1  = 0;                                  // Reset Timer1
    TCCR1B = 2;                                  // Enable Timer1 module with 1/8 prescaler ( 2 ticks every 1 us)
    nec_state = 1;                               // Next state: end of 9ms pulse (start of 4.5ms space)
    i = 0;
    return;
   case 1 :                                      // End of 9ms pulse
    if((timer_value > 19000) || (timer_value < 17000)){         // Invalid interval ==> stop decoding and reset
      nec_state = 0;                             // Reset decoding process
      TCCR1B = 0;                                // Disable Timer1 module
    }
    else
      nec_state = 2;                             // Next state: end of 4.5ms space (start of 562??s pulse)
    return;
   case 2 :                                      // End of 4.5ms space
    if((timer_value > 10000) || (timer_value < 8000)){
      nec_state = 0;                             // Reset decoding process
      TCCR1B = 0;                                // Disable Timer1 module
    }
    else
      nec_state = 3;                             // Next state: end of 562??s pulse (start of 562??s or 1687??s space)
    return;
   case 3 :                                      // End of 562??s pulse
    if((timer_value > 1400) || (timer_value < 800)){           // Invalid interval ==> stop decoding and reset
      TCCR1B = 0;                                // Disable Timer1 module
      nec_state = 0;                             // Reset decoding process
    }
    else
      nec_state = 4;                             // Next state: end of 562??s or 1687??s space
    return;
   case 4 :                                      // End of 562??s or 1687??s space
    if((timer_value > 3600) || (timer_value < 800)){           // Time interval invalid ==> stop decoding
      TCCR1B = 0;                                // Disable Timer1 module
      nec_state = 0;                             // Reset decoding process
      return;
    }
    if( timer_value > 2000)                      // If space width > 1ms (short space)
      bitSet(nec_code, (31 - i));                // Write 1 to bit (31 - i)
    else                                         // If space width < 1ms (long space)
      bitClear(nec_code, (31 - i));              // Write 0 to bit (31 - i)
    i++;
    if(i > 31){                                  // If all bits are received
      nec_ok = 1;                                // Decoding process OK
      detachInterrupt(0);                        // Disable external interrupt (INT0)
      return;
    }
    nec_state = 3;                               // Next state: end of 562??s pulse (start of 562??s or 1687??s space)
  }
}
 
ISR(TIMER1_OVF_vect) {                           // Timer1 interrupt service routine (ISR)
  nec_state = 0;                                 // Reset decoding process
  TCCR1B = 0;                                    // Disable Timer1 module
}
 
void loop() {
  if(nec_ok){                                    // If the mcu receives NEC message with successful
    nec_ok = 0;                                  // Reset decoding process
    nec_state = 0;
    TCCR1B = 0;                                  // Disable Timer1 module
    address = nec_code >> 16;
    command = nec_code >> 8;
    inv_command = nec_code;
    Serial.println(address);
    sprintf(text, "%04X", address);
    lcd.setCursor(10, 0);
    lcd.print(text);                             // Display address in hex format
    sprintf(text, "%02X", command);
    Serial.println(command);
    lcd.setCursor(6, 1);
    lcd.print(text);                             // Display command in hex format
    sprintf(text, "%02X", inv_command);
    lcd.setCursor(14, 1);
    lcd.print(text);                             // Display inverted command in hex format
    attachInterrupt(0, remote_read, CHANGE);     // Enable external interrupt (INT0)
  }
}
