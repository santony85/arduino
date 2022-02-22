#include <IRremote.h>

uint16_t sAddress = 0x0102;
uint8_t sCommand = 0x34;
uint8_t sRepeats = 5;

void setup() {
  // put your setup code here, to run once:
      pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    IrSender.begin(true);

}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println(F("Send RC5"));
    IrSender.sendRC5(0x30,20);
    sRepeats++;
    // clip repeats at 4
    if (sRepeats > 4) {
        sRepeats = 0;
    }
    delay(2000);

}
