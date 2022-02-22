/*
 * Initial Author: ryand1011 (https://github.com/ryand1011)
 *
 * Reads data written by a program such as "rfid_write_personal_data.ino"
 *
 * See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
 *
 * Uses MIFARE RFID card using RFID-RC522 reader
 * Uses MFRC522 - Library
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>
#include <MFRC522.h>

#include <Adafruit_NeoPixel.h>

#define PIN 6

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);
uint32_t c;
uint32_t defcolor=strip.Color(0, 0, 0);
int incomingByte = 0;
bool isok=false;
//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  //Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
strip.begin();               // initialize strip
strip.show();                // Update all LEDs (= turn OFF, since none of them have been set yet!)  // define the variable c as RED (R,G,B)
strip.setPixelColor(0, defcolor);  // set LED 10 to the color in variable c (red)
strip.setPixelColor(1, defcolor);  // set LED 10 to the color in variable c (red)
strip.show();  
}

//*****************************************************************************************//
void loop() {

  if (Serial.available() > 0) {
    
    incomingByte = Serial.read();
    if(incomingByte == 'a'){ //in process
      delay(1000);
      c = strip.Color(0, 0, 0);  // define the variable c as RED (R,G,B)
      strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
      strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
      strip.show(); 
      }
    else if(incomingByte == 'z'){ // reset = ok
      c = strip.Color(100, 100, 0);  // define the variable c as RED (R,G,B)
      strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
      strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
      strip.show(); 
      }
    else if(incomingByte == 'e'){ //error machine
      isok=false;
      c = strip.Color(255, 0, 0);  // define the variable c as RED (R,G,B)
      strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
      strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
      strip.show(); 
      }
    else if(incomingByte == 'c'){ // clear
      c = strip.Color(100, 100, 0);  // define the variable c as RED (R,G,B)
      strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
      strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
      strip.show(); 
      isok=true;
      }
      
  }


  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;
  //-------------------------------------------
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  byte buffer1[18];
  block = 4;
  len = 18;
  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  byte buffer2[18];
  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  String myString = String((char*)buffer2);
  myString = myString.substring(0,16);

if(isok){  
  if(myString == "FONTEKO-2021-001"){
    c = strip.Color(0, 255, 0);  // define the variable c as RED (R,G,B)
    strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
    strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
    strip.show(); 
    Serial.println("1200000045002000");
    //Serial.println(myString);
    }

   else if(myString == "FONTEKO-TECH-001"){
    c = strip.Color(0, 255, 0);  // define the variable c as RED (R,G,B)
    strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
    strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
    strip.show(); 
    Serial.println("tech");
    }  
  else {
    c = strip.Color(255, 0, 0);  // define the variable c as RED (R,G,B)
    strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
    strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
    strip.show(); 
    delay(1000);
    }
}
else if(myString == "FONTEKO-TECH-001"){
    c = strip.Color(0, 255, 0);  // define the variable c as RED (R,G,B)
    strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
    strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
    strip.show(); 
    Serial.println("tech");
    }  

  
  delay(100); //change value if you want to read cards faster
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
    
}
//*****************************************************************************************//
