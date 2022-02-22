#include <SIM808.h>
#include <SoftwareSerial.h>
#include <ArduinoLog.h>

#define SIM_RST    15 ///< SIM808 RESET
#define SIM_RX    17 ///< SIM808 RXD
#define SIM_TX    16 ///< SIM808 TXD
#define SIM_PWR   9 ///< SIM808 PWRKEY
#define SIM_STATUS  8 ///< SIM808 STATUS

SoftwareSerial simSerial(SIM_TX,SIM_RX);

#define SIM808_BAUDRATE 9600    ///< Control the baudrate use to communicate with the SIM808 module
#define SERIAL_BAUDRATE 115200   ///< Controls the serial baudrate between the arduino and the computer
#define NO_FIX_GPS_DELAY 1000   ///< Delay between each GPS read when no fix is acquired
#define FIX_GPS_DELAY  3000    ///< Delay between each GPS read when a fix is acquired
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);

#define POSITION_SIZE   128 
char position[POSITION_SIZE];

#if defined(__AVR__)
    typedef __FlashStringHelper* __StrPtr;
#else
    typedef const char* __StrPtr;
#endif

String slat, slon,sspeed,salt,scourse,ssattelites;

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);
    Log.notice(S_F("Powering on SIM808..." NL));
    //sim808.powerOnOff(true);
    sim808.init();
    Log.notice(S_F("Powering on SIM808's GPS..." NL));
    sim808.powerOnOffGps(true);
    //sim808.print("AT+CSMP=17,167,0,0");  // set this parameter if empty SMS received
    delay(100);
    sim808.print("AT+CMGF=1\r");
    delay(400);
    sim808.print("ATS0=1\r");
    delay(100);
}

void loop(){
  handleGPS();
  handleSMS();
  while(sim808.available()){
    Serial.println(sim808.readString());
    }
  while(Serial.available()){
    sim808.println(Serial.readString());
    }
  //delay(3000);
}
void sendSMS(){
  //bool success = sim808.sendSms(number, buffer);  
}

void handleSMS(){
  char messageIndex = sim808.isSMSunread();
  //Serial.println(messageIndex);
}

void handleGPS(){
    SIM808GpsStatus status = sim808.getGpsStatus(position, POSITION_SIZE);
    if(status < SIM808GpsStatus::Fix) {
        Log.notice(S_F("No fix yet..." NL));
        delay(NO_FIX_GPS_DELAY);
        return;
    }

    uint16_t sattelites;
    float lat, lon,speed,alt,course;
    
    __StrPtr state;

    if(status == SIM808GpsStatus::Fix) state = S_F("Normal");
    else state = S_F("Accurate");

    sim808.getGpsField(position, SIM808GpsField::GnssUsed, &sattelites);
    sim808.getGpsField(position, SIM808GpsField::Latitude, &lat);
    sim808.getGpsField(position, SIM808GpsField::Longitude, &lon);
    sim808.getGpsField(position, SIM808GpsField::Speed, &speed);
    sim808.getGpsField(position, SIM808GpsField::Altitude, &alt);
    sim808.getGpsField(position, SIM808GpsField::Course, &course);

    ssattelites=String(sattelites);
    slat=String(lat,6);
    slon=String(lon,6);
    sspeed=String(speed,2);
    salt=String(alt,2);
    scourse=String(course,2);
    Serial.println(ssattelites);
    Serial.println(slat);
    Serial.println(slon);
    Serial.println(sspeed);
    Serial.println(salt);
    Serial.println(scourse);

    //Serial.println(position);    
    delay(FIX_GPS_DELAY);
}
