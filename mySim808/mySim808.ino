/*!
 * @file  SIM808_GetGPS.ino
 * @brief  Get GPS data
 * @details  1. This example is used to test SIM808 GPS/GPRS/GSM Shield's reading GPS data.
 * @n        2. Open the SIM808_GetGPS example or copy these code to your project
 * @n        3. Download and dial the function switch to Arduino
 * @n        4. open serial helper
 * @n        4. Place it outside, waiting for a few minutes and then it will send GPS data to serial
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [Jason](jason.ling@dfrobot.com)
 * @maintainer  [qsjhyy](yihuan.huang@dfrobot.com)
 * @version  V1.0
 * @date  2022-02-08
 * @url  https://github.com/DFRobot/DFRobot_SIM808
 */
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

#define POSITION_SIZE   128     ///< Size of the position buffer
#define NL  "\n"

#define MESSAGE_LENGTH 20
char gprsBuffer[64];
char *s = NULL;

char phone[16];
char datetime[24];

#define MESSAGE_LENGTH2 160
char message[MESSAGE_LENGTH2];
int messageIndex = 0;

#if defined(__AVR__)
    typedef __FlashStringHelper* __StrPtr;
#else
    typedef const char* __StrPtr;
#endif

SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);
char position[POSITION_SIZE];

    String slat, slon,sspeed,salt,scourse,ssattelites;

void setup() {
  //
    Serial.begin(SERIAL_BAUDRATE);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);
    Log.notice(S_F("Powering on SIM808..." NL));
    //sim808.powerOnOff(true);
    sim808.init();
    Log.notice(S_F("Powering on SIM808's GPS..." NL));
    sim808.powerOnOffGps(true);
}

void loop() {
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
    /*Log.notice(S_F("Fix type: %S" NL), state);
    Log.notice(S_F("Sattelites used : %d" NL), sattelites);
    Log.notice(S_F("Latitude : %F" NL), lat);
    Log.notice(S_F("Longitude : %F" NL), lon);
    
    Log.notice(S_F("Speed : %F" NL), speed);
    Log.notice(S_F("Altitude : %F" NL), alt);
    Log.notice(S_F("Course : %F" NL), course);*/

    
    delay(FIX_GPS_DELAY);
}
