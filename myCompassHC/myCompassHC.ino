#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(9600);
  compass.init();
  //compass.setCalibration(-1157, 880, -1142, 1157, -2231, 0);
}

void loop() {

  int16_t x, y, z, a, b;
  char myArray[3];
  
  compass.read();
  
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  
  a = compass.getAzimuth();
  
  b = compass.getBearing(a);

  compass.getDirection(myArray, a);
  
  
  Serial.print("X: ");
  Serial.print(x);

  Serial.print(" Y: ");
  Serial.print(y);

  Serial.print(" Z: ");
  Serial.print(z);

  Serial.print(" Azimuth: ");
  Serial.print(a);

  Serial.print(" Bearing: ");
  Serial.print(b);

  Serial.print(" Direction: ");
  Serial.print(myArray[0]);
  Serial.print(myArray[1]);
  Serial.print(myArray[2]);

  Serial.println();
  float angle = atan2((double)x, (double)y);
  Serial.print("angle:\t");
  Serial.println(angle * 180 / PI);

  delay(250);
}
