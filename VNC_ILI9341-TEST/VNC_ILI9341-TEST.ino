/*
 * VNC_ILI9341.ino
 *
 *  Created on: 07.01.2015
 *
 * required librarys:
 *  - SPI (arduino core)
 *  - WiFi (arduino core)
 *  - Adafruit_GFX (https://github.com/adafruit/Adafruit-GFX-Library)
 *  - Adafruit_ILI9341 (https://github.com/Links2004/Adafruit_ILI9341)
 *  - arduinoVNC (https://github.com/Links2004/arduinoVNC)
 */
#include "SPIFFS.h"
#include <JPEGDecoder.h>

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>

#else
#include <WiFi.h>
#endif
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include <VNC_ILI9341.h>
#include <VNC.h>


#include <HTTPClient.h>




// ILI9341
#define TFT_DC      (4)
#define TFT_CS      (15)
#define TFT_MOSI (23) //-          
#define TFT_CLK (18) //-
#define TFT_RST (2) //-

#define TFT_MISO (19) //NOT 

const char * vnc_ip = "192.168.1.43";
const uint16_t vnc_port = 80;
const char * vnc_pass = "12345678";

const char* ssid = "Livebox-4296";
const char* password = "Santony85";

ILI9341VNC tft = ILI9341VNC(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
arduinoVNC vnc = arduinoVNC(&tft);
boolean SPIFFSInited = false;

void TFTnoWifi(void) {
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, ((240 / 2) - (5 * 8)));
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(5);
    tft.println("NO WIFI!");
    tft.setTextSize(2);
    tft.println();
}


void TFTnoVNC(void) {
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, ((240 / 2) - (4 * 8)));
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(4);
    tft.println("connect VNC");
    tft.setTextSize(2);
    tft.println();
    tft.print(vnc_ip);
    tft.print(":");
    tft.println(vnc_port);
}

void setup(void) {
    //
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();
    //Bridge.begin(115200);
    // Init ILI9341
    tft.begin();
    //delay(3000);
    tft.setRotation(1);
delay(3000);
    Serial.println(testText());
    
delay(3000);

#ifdef ESP8266
    // disable sleep mode for better data rate
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    TFTnoWifi();
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    TFTnoVNC();

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println(F("[SETUP] VNC..."));

    //vnc.begin(vnc_ip, vnc_port);
    //vnc.setPassword(vnc_pass); // optional
}

void loop() {
    if(WiFi.status() != WL_CONNECTED) {
        //vnc.reconnect();
        TFTnoWifi();
        delay(100);
    } else {
            HTTPClient http;
            http.begin("http://192.168.1.43/jpg/image.jpg"); //Specify the URL
            int httpCode = http.GET(); 
            if (httpCode > 0) { //Check for the returning code
 
               String payload = http.getString();
               Serial.println(httpCode);
               Serial.println(payload);
            }
 
            else {
            Serial.println("Error on HTTP request");
             }
 
            http.end(); //Free the resources
            }
}

unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}


/*====================================================================================
  This sketch contains support functions to render the Jpeg images.

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/

// Return the minimum of two values a and b
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

//====================================================================================
//   This function opens the Filing System Jpeg image file and primes the decoder
//====================================================================================
void drawFSJpeg(const char *filename, int xpos, int ypos) {

  Serial.println("=====================================");
  Serial.print("Drawing file: "); Serial.println(filename);
  Serial.println("=====================================");

  // Open the file (the Jpeg decoder library will close it)
  fs::File jpgFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpgFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  if ( !jpgFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  // To initialise the decoder and provide the file, we can use one of the three following methods:
  //boolean decoded = JpegDec.decodeFsFile(jpgFile); // We can pass the SPIFFS file handle to the decoder,
  //boolean decoded = JpegDec.decodeSdFile(jpgFile); // or we can pass the SD file handle to the decoder,
  boolean decoded = JpegDec.decodeFsFile(filename);  // or we can pass the filename (leading / distinguishes SPIFFS files)
                                                     // The filename can be a String or character array
  if (decoded) {
    // print information about the image to the serial port
    jpegInfo();

    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

//====================================================================================
//   Decode and paint onto the TFT screen
//====================================================================================
void jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      for (int h = 1; h < win_h-1; h++)
      {
        memcpy(pImg + h * win_w, pImg + (h + 1) * mcu_w, win_w << 1);
      }
    }


    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height())
    {
      tft.drawRGBBitmap(mcu_x, mcu_y, pImg, win_w, win_h);
    }

    // Stop drawing blocks if the bottom of the screen has been reached,
    // the abort function will close the file
    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();

  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime;

  // print the results to the serial port
  Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
  Serial.println("=====================================");

}

//====================================================================================
//   Send time taken to Serial port
//====================================================================================
void jpegInfo() {
  Serial.println(F("==============="));
  Serial.println(F("JPEG image info"));
  Serial.println(F("==============="));
  Serial.print(F(  "Width      :")); Serial.println(JpegDec.width);
  Serial.print(F(  "Height     :")); Serial.println(JpegDec.height);
  Serial.print(F(  "Components :")); Serial.println(JpegDec.comps);
  Serial.print(F(  "MCU / row  :")); Serial.println(JpegDec.MCUSPerRow);
  Serial.print(F(  "MCU / col  :")); Serial.println(JpegDec.MCUSPerCol);
  Serial.print(F(  "Scan type  :")); Serial.println(JpegDec.scanType);
  Serial.print(F(  "MCU width  :")); Serial.println(JpegDec.MCUWidth);
  Serial.print(F(  "MCU height :")); Serial.println(JpegDec.MCUHeight);
  Serial.println(F("==============="));
}

//====================================================================================
//   Open a Jpeg file and dump it to the Serial port as a C array
//====================================================================================
void createArray(const char *filename) {

  fs::File jpgFile;  // File handle reference for SPIFFS
  //  File jpgFile;  // File handle reference For SD library
  
  if ( !( jpgFile = SPIFFS.open( filename, "r"))) {
    Serial.println(F("JPEG file not found"));
    return;
  }

  uint8_t data;
  byte line_len = 0;
  Serial.println("// Generated by a JPEGDecoder library example sketch:");
  Serial.println("// https://github.com/Bodmer/JPEGDecoder");
  Serial.println("");
  Serial.println("#if defined(__AVR__)");
  Serial.println("  #include <avr/pgmspace.h>");
  Serial.println("#endif");
  Serial.println("");
  Serial.print("const uint8_t ");
  while (*filename != '.') Serial.print(*filename++);
  Serial.println("[] PROGMEM = {"); // PROGMEM added for AVR processors, it is ignored by Due

  while ( jpgFile.available()) {

    data = jpgFile.read();
    Serial.print("0x"); if (abs(data) < 16) Serial.print("0");
    Serial.print(data, HEX); Serial.print(",");// Add value and comma
    line_len++;
    if ( line_len >= 32) {
      line_len = 0;
      Serial.println();
    }

  }

  Serial.println("};\r\n");
  // jpgFile.seek( 0, SeekEnd);
  jpgFile.close();
}
//====================================================================================
