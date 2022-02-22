#include <SoftwareSerial.h>

#include <TinyGPS.h>
TinyGPS gps;



SoftwareSerial ss(17, 16);

#include "heltec.h"
#include "image.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

int volt = 36; 
int amp = 37; 

// 5V/1024 or 0.0049V = 4.9mV/analog unit 0,001220703 0,000805664
#define MV_TO_ANALOG	0.92
// from AttoPilot docs
// http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Current/DC%20Voltage%20and%20Current%20Sense%20PCB%20with%20Analog%20Output.pdf
#define SENSOR_180_MV_TO_VOLT	63.69
#define SENSOR_90_MV_TO_VOLT	63.69	
#define SENSOR_13_MV_TO_VOLT	242.3

#define SENSOR_180_MV_TO_CURRENT	18.30
#define SENSOR_90_MV_TO_CURRENT	36.60	
#define SENSOR_13_MV_TO_CURRENT	73.20

// adjust this to whatever you are comfortable with
#define LOW_LIPO_VOLTAGE		11.0


static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

String lat="",lng="";
int poslatlng=0;
String va="";


void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void setup() {

  
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  logo();
  delay(1500);
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 0, "Informations :");
  Heltec.display->display();
  delay(1000);
  Heltec.display->setFont(ArialMT_Plain_10);
  ss.begin(9600);
  Heltec.display->drawString(0, 48, "V : 12.52");
  Heltec.display->display();
}


void LoraSnd(){

    Heltec.display->clear();
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(0, 0, "Informations :");
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawStringMaxWidth(0, 20,128, "Lat : "+lat);
    Heltec.display->drawStringMaxWidth(0, 34,128, "Lng : "+lng);
    Heltec.display->drawString(0, 48, va);
    Heltec.display->display();
    LoRa.beginPacket();
    LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
    LoRa.print(lat+","+lng);
    LoRa.endPacket();
    //delay(2000);
}


void loop() {
  float flat, flon;
  unsigned long age, date, time;
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);

  /*float raw_V = analogRead(volt);
	// convert analog arduino units to mV units 
	float raw_mV = raw_V * MV_TO_ANALOG;
	// convert mV units to sensor values for voltage
	float v_sensor = raw_mV / SENSOR_180_MV_TO_VOLT;
	// get the analog output from the sensor
	float raw_I = analogRead(amp);
	// convert analog arduino units to mV units 
	float raw_i_mV = raw_I * MV_TO_ANALOG;
	// convert mV units to sensor values for current
	float i_sensor = raw_i_mV / SENSOR_180_MV_TO_CURRENT;

	// voltage is too low so sound alarm
	if(v_sensor < LOW_LIPO_VOLTAGE)
	{
		digitalWrite(8, HIGH);
	}
  va = (String)v_sensor+" V -> "+i_sensor+" A";

	Serial.println(va);*/


  smartdelay(1000);

}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
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
    if(poslatlng==0){
     poslatlng=1;
     lat=String(val,prec); 
     //Serial.println(lat);

    }
    else if(poslatlng==1){
     poslatlng=0;
     lng=String(val,prec);  
     //Serial.println(lng);
     LoraSnd();
    }

    

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