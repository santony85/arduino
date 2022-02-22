#include <SoftwareSerial.h>
SoftwareSerial ss(17, 16);
#include <JY901.h>
#include "heltec.h"
#include "image.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

#include <TinyGPS.h>
TinyGPS gps;
SoftwareSerial ssgps(13, 21);
String lat="",lng="";
int poslatlng=0;
String va="";


void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void setup() 
{
	//Serial.begin(115200);
  ss.begin(9600);
	JY901.attach(ss);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  logo();
  delay(1500);
  ssgps.begin(9600);

}

void loop() 
{
	JY901.receiveSerialData();
  


	//print received data. Data was received in serialEvent;
	/*Serial.print("Time:20");
	Serial.print(JY901.getTime("year"));
	Serial.print("-");
	Serial.print(JY901.getTime("month"));
	Serial.print("-");
	Serial.print(JY901.getTime("day"));
	Serial.print(" ");
	Serial.print(JY901.getTime("hour"));
	Serial.print(":");
	Serial.print(JY901.getTime("minute"));
	Serial.print(":");
	Serial.println((float)JY901.getTime("second")+(float)JY901.getTime("milisecond")/1000);*/

							 
	Serial.print("Acc:");
	Serial.print(JY901.getAccX());
	Serial.print(" ");
	Serial.print(JY901.getAccY());
	Serial.print(" ");
	Serial.print(JY901.getAccZ());
	Serial.print("\n");

	
	Serial.print("Gyro:");
	Serial.print(JY901.getGyroX());
	Serial.print(" ");
	Serial.print(JY901.getGyroY());
	Serial.print(" ");
	Serial.print(JY901.getGyroZ());
	Serial.print("\n");
	
	Serial.print("Mag:");
	Serial.print(JY901.getMagX());
	Serial.print(" ");
	Serial.print(JY901.getMagY());
	Serial.print(" ");
	Serial.print(JY901.getMagZ());
	Serial.print("\n");

	Serial.print("Angle:");
	Serial.print(JY901.getRoll());
	Serial.print(" ");
	Serial.print(JY901.getPitch());
	Serial.print(" ");
	Serial.print(JY901.getYaw());
	Serial.print("\n");

	/*Serial.print("Pressure:");
	Serial.println(JY901.getPressure());
	Serial.print("Altitude:");
	Serial.println(JY901.getAltitude()/100.0);

	
	Serial.print("DStatus:");
	Serial.print(JY901.getD0Status());
	Serial.print(" ");
	Serial.print(JY901.getD1Status());
	Serial.print(" ");
	Serial.print(JY901.getD2Status());
	Serial.print(" ");
	Serial.print(JY901.getD3Status());
	Serial.print("\n");


	Serial.print("Longitude:");
	Serial.print(JY901.getLon()/10000000);
	Serial.print("Deg");
	Serial.print((double)(JY901.getLon() % 10000000)/1e5);
	Serial.println("m");

	Serial.print("Lattitude:");
	Serial.print(JY901.getLat()/10000000);
	Serial.print("Deg");
	Serial.print((double)(JY901.getLat() % 10000000)/1e5);
	Serial.println("m");

	
	Serial.print("GPSHeight:");
	Serial.print(JY901.getGPSH());
	Serial.println("m");
	Serial.print("GPSYaw:");
	Serial.print(JY901.getGPSY());
	Serial.println("Deg:");
	Serial.print("GPSV:");
	Serial.print(JY901.getGPSV());
	Serial.println("km/h");
*/

   float flat, flon;
  unsigned long age, date, time, chars = 0;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  print_date(gps);
  //print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);



  //gps.stats(&chars, &sentences, &failed);
  //print_int(chars, 0xFFFFFFFF, 6);
  //print_int(sentences, 0xFFFFFFFF, 10);
  //print_int(failed, 0xFFFFFFFF, 9);
  Serial.println();
  
  smartdelay(1000);
  }



}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ssgps.available())
      gps.encode(ssgps.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}
