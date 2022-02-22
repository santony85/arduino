#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
uint32_t c;
uint32_t defcolor=strip.Color(0, 255, 0);

void setup() {
  // put your setup code here, to run once:
  strip.begin();               // initialize strip
strip.show();                // Update all LEDs (= turn OFF, since none of them have been set yet!)  // define the variable c as RED (R,G,B)
strip.setPixelColor(0, defcolor);  // set LED 10 to the color in variable c (red)
strip.show();  

}

void loop() {
  // put your main code here, to run repeatedly:

}
