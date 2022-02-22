
// button define
#define LONG_PRESS 2
#define SHORT_PRESS 1
#define JUST_PRESS_MODE 1
#define LONG_PRESS_MODE 2

#include "button.h"


/*button *but;
button *but2;
button *but3;
button *but4;*/
int b1=4;
int b2=7;
int b3=10;
int b4=11;

  int v1 = 0;
  int v2 = 0;
  int v3 = 0;
  int v4 = 0;

void setup() {
  // put your setup code here, to run once:
  /*but = new button(4);
  but2 = new button(7);
  but3 = new button(10);
  but4 = new button(10);*/
  pinMode( b1 ,INPUT_PULLUP );
  pinMode( b2 ,INPUT_PULLUP );
  pinMode( b3 ,INPUT_PULLUP );
  pinMode( b4 ,INPUT_PULLUP );
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(57600);
  Serial.setTimeout(20);
  delay(20);

}

void loop() {
  // put your main code here, to run repeatedly:
  /*but->update();
  but2->update();
  but3->update();
  but4->update();*/
  digitalWrite(2, false);
  digitalWrite(5, false);
  digitalWrite(8, false);
  digitalWrite(13, false);

  v1 = !digitalRead(b1);
  v2 = !digitalRead(b2);
  v3 = !digitalRead(b3);
  v4 = !digitalRead(b4);
  if(v1)digitalWrite(2, true);
  if(v2)digitalWrite(5, true);
  if(v3)digitalWrite(8, true);
  if(v4)digitalWrite(13, true);
  //delay(1);
}
