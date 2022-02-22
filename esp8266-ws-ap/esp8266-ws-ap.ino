#include <WS2812FX.h>
#define LED_PIN 2                       // 0 = GPIO0, 2=GPIO2
#define LED_COUNT 10

#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 128
#define DEFAULT_SPEED 1000
#define DEFAULT_MODE FX_MODE_COLOR_WIPE

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\nStarting...");

  //modes.reserve(5000);
  //modes_setup();

  Serial.println("WS2812FX setup");
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();

}

void loop() {
  // put your main code here, to run repeatedly:
  ws2812fx.service();

}
