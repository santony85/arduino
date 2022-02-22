/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo; 
Servo myservo2;
// create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

 int ledPin = 13;
 int joyPin1 = 0;                 // slider variable connecetd to analog pin 0
 int joyPin2 = 1;                 // slider variable connecetd to analog pin 1
 int value1 = 0;                  // variable to read the value from the analog pin 0
 int value2 = 0;                  // variable to read the value from the analog pin 1



 int tabS1[]={90,91,  92,  93,  94,  95,  96,  97,  102, 106, 102, 98,  96,  99,  102, 110};
 int tabS2[]={120, 108, 100, 102, 104, 90,  95,  97,  100, 100, 100, 106, 108, 110, 116, 120};

void setup() {


  Serial.begin(115200);

// Calibration de la valeur (0, 0) du joystick


  myservo2.attach(10); 
  myservo.attach(9);
  // attaches the servo on pin 9 to the servo object
  myservo.write(tabS1[0]);    
  myservo2.write(tabS2[0]);
  
}




void loop() {
  /*for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
   // myservo.write(pos);    
    //myservo2.write(pos); // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    //myservo.write(pos);      
    //myservo2.write(pos);// tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }*/



/*int posServo1=0;
int tmpX = 0;
if(rawX <=0) posServo1 = 90-(int)(rawX/6);
else posServo1 = 90+(int)(rawX/4);

myservo.write(posServo1); */

  value1 = analogRead(joyPin1);  
  int mapX = map(value1, 0, 1024, 0, 180);
  delay(100);            
  // reads the value of the variable resistor
  value2 = analogRead(joyPin2);  
  int mapY = map(value2, 0, 1024, 0, 180);
  Serial.println(value2 );
  //Serial.println(mapX);
  for (int i = 0; i < 16; i++){
      myservo.write(tabS1[i]);    
  myservo2.write(tabS2[i]);
  delay(500);
  }

  //myservo.write(mapX);
  //myservo2.write(mapY);
  /*myservo.write(tabS1[0]);    
  myservo2.write(tabS2[0]);
  delay(500);
  myservo.write(tabS1[1]);    
  myservo2.write(tabS2[1]);
  delay(500);
  myservo.write(tabS1[2]);    
  myservo2.write(tabS2[2]);
    delay(500);
  myservo.write(tabS1[3]);    
  myservo2.write(tabS2[3]);
  delay(500);*/
  

}
