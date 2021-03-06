/*
  Adeept Distance module library V1.0
  2015 Copyright (c) Adeept Technology Inc.  All right reserved.
  Author: TOM
*/

#ifndef ADEEPT_DISTANCE_H_
#define ADEEPT_DISTANCE_H_

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif

class Adeept_Distance
{
	public:
		Adeept_Distance();
		void begin();
		void begin(int echoPin, int trigPin);
		
		int getDistanceTime();
		int getDistanceCentimeter();
		int getDistanceInch();

		boolean isCloser(int threshold);
		boolean isFarther(int threshold);

		void setAveraging(int avg);    

	private:
		int _trigPin;
		int _echoPin;
		int _average;
		long _duration;
};
#endif
