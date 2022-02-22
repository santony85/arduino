#include <SPI.h>
#include <MFRC522.h>

#include <Adafruit_NeoPixel.h>

#define PIN 6

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);
uint32_t c;
uint32_t defcolor=strip.Color(100, 100, 0);
int incomingByte = 0;
bool isok=true;


void setup() {
  // put your setup code here, to run once:
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

void loop() {
  // put your main code here, to run repeatedly:

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
  if(myString == "FONTEKO-2021-001" && !isok){
    isok = !isok;
    c = strip.Color(0, 255, 0);  // define the variable c as RED (R,G,B)
    strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
    strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
    strip.show(); 
    delay(5000);  
    }
    else{
      isok = !isok;
      c = strip.Color(255, 0, 0);  // define the variable c as RED (R,G,B)
      strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
      strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
      strip.show(); 
      delay(5000);
    }
  

  Serial.println(myString);

      c = strip.Color(100, 100, 0);  // define the variable c as RED (R,G,B)
    strip.setPixelColor(0, c);  // set LED 10 to the color in variable c (red)
    strip.setPixelColor(1, c);  // set LED 10 to the color in variable c (red)
    strip.show(); 

  delay(100); //change value if you want to read cards faster
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

}
