
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>
#include <JY901.h>

#include <MicroNMEA.h>
#include "EEPROM.h"
#define ssGSM Serial2


//#include "FS.h"
//#define LittleFS LITTLEFS
//#include <LITTLEFS.h> 

String cimi, gsn;
int rx0 = 18;    // D7 pushbutton connected to digital pin 7
int rx1 = 5;    // D6 pushbutton connected to digital pin 7
int vrx0 = 0;
int vrx1 = 0;
int isRemote = 0;
//SoftwareSerial ssMPU(12, 14); //D5,D4
int RST_KEY = 19;//D8
#define LOG_RATE 10000
unsigned long lastLog = 0;

int isInit = 0;
String msgIn;
char* command;
//String mdata;
String data[10];

#define ONBOARD_LED  2

float Accel_x, Accel_y, Accel_z;
float Gyro_x, Gyro_y, Gyro_z;
float Mag_x, Mag_y, Mag_z;
float Q0, Q1, Q2, Q3;
float D;

float Roll, Pitch, Yaw, Heading;
bool ModuleState = true;
String phone[5] = {"+33659401246", " ", " ", " ", " "};

int isAlarm = 0;
int isTracking = 0;

String lat = "51.508131", lng = "-1.128002", alts = "", gpsdate = "10:00:00", speed = "0.00";

String allat = "51.508131", allng = "-1.128002", algpsdate = "10:00:00";
String alroll = "0.0", alpitch = "0.0", alyaw = "0.0";


double flat, flon, falt, alflat = 51.508131, alflon = -1.128002, fspeed;

String myIp;
int isStart = 0;

long lastUpdate, deltat, Now;

//MicroNMEA library structures
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
unsigned long startTime, mpuTime;
#include "mynmea.h"
//#include "mympu.h"


void writeData(String datar){
  EEPROM.writeString(0, datar);
  EEPROM.commit();
  }
void readData(){

   String val = EEPROM.readString(0);
   char buf[1024];
   val.toCharArray(buf, sizeof(buf));
   char *p = buf;
   char *str;
   int a=0;
    
   while ((str = strtok_r(p, ";", &p)) != NULL){ // delimiter is the semicolon
           if(a<5)phone[a++]=str;
      else if(a==5){allat=str;a++;}
      else if(a==6){allng=str;a++;}
      else if(a==7){algpsdate=str;a++;}
      else if(a==8){alroll=str;a++;}
      else if(a==9){alpitch=str;a++;}
      else if(a==10){alyaw=str;a++;}
    }
    //String valt="+33659401246;"+phone[1]+";"+phone[2]+";"+phone[3]+";"+phone[4]+";"+allat+";"+allng+";"+algpsdate+";"+alroll+";"+alpitch+";"+alyaw+"\r";
    
    Serial.println(val);
    Serial.println("read");
  }
void printUnknownSentence(MicroNMEA& nmea)
{
  Serial.println();
  Serial.print("Unknown sentence: ");
  Serial.println(nmea.getSentence());
}


void setup() {
  pinMode(RST_KEY, OUTPUT);
  pinMode(ONBOARD_LED,OUTPUT);
  Serial.begin(115200);
  digitalWrite(ONBOARD_LED,LOW);
  Serial.println();
  Serial.println("Start");
  ssGSM.begin(115200);
  Serial.println();
  //ssMPU.begin(9600);
  JY901.startIIC();
  /*JY901.caliIMU();
  delay(5000);
  JY901.quitCali();
  JY901.caliMag();
  delay(5000);
  JY901.quitCali();*/

  Serial.println("Start GSM");
  nmea.setUnknownSentenceHandler(printUnknownSentence);
  // Clear the list of messages which are sent.
  MicroNMEA::sendSentence(ssGSM, "$PORZB");
  // Send only RMC and GGA messages.
  MicroNMEA::sendSentence(ssGSM, "$PORZB,RMC,1,GGA,1");

  if (!EEPROM.begin(300)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    //delay(1000);
    //ESP.restart();
    }
  
  readData();
  digitalWrite(RST_KEY, LOW);
  delay(200);
  digitalWrite(RST_KEY, HIGH);
  delay(1000);
  pinMode(rx0, INPUT);
  pinMode(rx1, INPUT);
  //JY901.attach(ssMPU);
  delay(1000);
  startTime = millis();
  mpuTime = millis();
  Serial.println("ready to go");
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void loop() {
  
  vrx0 = digitalRead(rx0);
  vrx1 = digitalRead(rx1);
  int msum = (vrx0 * 10) + vrx1;
  if (msum > 0 && isRemote == 0) {
    //Serial.println(msum);
    if (msum == 1) {
      isRemote = 1;
      if (isAlarm == 0) {
        setAlarm("#alarm on");
      }
      else setAlarm("#alarm off");
    }
    else if (msum == 11) {

    }
    else if (msum == 10) {
      ModuleState = false;
      sendSmsToAllInfo("");//+makeGyroData());
      ModuleState = true;
    }
    delay(100);
  }



  if (Serial.available() > 0) {
    msgIn = Serial.readString();
    if (msgIn == "1") {
      ModuleState = false;
      ssGSM.flush();
      sendHttpPost();
      ModuleState = true;
    }
    else if (msgIn == "2") {
      ModuleState = false;
      sendTCP();
      ModuleState = true;
    }
    else if (msgIn == "3") {
      connectGSM("AT+CIPSTART=\"TCP\",\"51.68.125.223\",4205","OK");
    }  
    else {
      connectGSM(msgIn, "OK");
    }
  }

  if (ssGSM.available() > 0 && ModuleState) {
    String mdata = ssGSM.readString();
    if (mdata.indexOf("#info") >= 0) {
      ModuleState = false;
      ssGSM.flush();
      sendSmsToAllInfo("");//+makeGyroData());
      ModuleState = true;
      }
    else if (mdata.indexOf("#alarm") >= 0) {
      setAlarm(mdata);
      }
    else if (mdata.indexOf("+CIPRCV") >= 0) {
      String gdata = mdata.substring(8);
      if (gdata.indexOf("#start") >= 0){
        isTracking = 1;
        Serial.println("start tracking"); 
        }
      else if (gdata.indexOf("#stop") >= 0){
        isTracking = 0;
        Serial.println("stop tracking");  
        }
      else if (gdata.indexOf("#get") >= 0){
        ModuleState = false;
        sendTCP();
        ModuleState = true;
        }
      //Serial.println(gdata);
      }
    else if (mdata.indexOf("+GPSRD") >= 0) {
      String gdata = mdata.substring(7);
      buildtrame(gdata);
      if(isTracking && isValid)sendTCP();
      if (isAlarm) {
        double dst = CalcDistance(alflat, alflon, flat, flon );
        if (dst > 5) {
          Serial.print(dst);
          Serial.println(" m DANGER !!!");
          //sendSmsToAll("DANGER !!!!");
        }
      }
      //ModuleState=true;
    }
    //wait ready
    else if (mdata.indexOf("+CREG: 1") >= 0) {
      //connectGSM("AT+IPR=115200", "OK");
      //ssGSM.end();
      //ssGSM.begin(115200);
      //initGSM();
      connectGSM("AT+CPMS=\"ME\"", "OK");
      connectGSM("AT+CMGD=1,4", "OK");
      connectGSM("AT+CMGF=1", "OK");
      connectGSM("AT+CNMI=2,2,0,0,0", "OK");
      connectGSM("AT+GPS=1", "OK");
      initGPRS();
      connectGSM("AT+GPSRD=7", "OK");
      //getMobileInfos();
      isInit = 1;
      
      //sendSmsToAll("Startup OK");
    }
    else {
      //Serial.println(mdata);
    }
    //yield();
  }
  if (isInit) {
    if (millis() - startTime >= 600000) {
      startTime = millis();
      ModuleState = false;
      sendHttpPost();
      ModuleState = true;
    }
    if (millis() - mpuTime >= 500) {
      mpuTime = millis();
      //digitalWrite(ONBOARD_LED,LOW);
      
    }
    makeMPUData();
    //if(isTracking)sendTCP();
  }

}

void sendTCP() {
  ModuleState=false;
      String mjson = JSONalarmPosition();
      connectGSM("AT+CIPSEND", "");
      delay(150);
      connectGSM("{"+mjson+"}", "");
      delay(10);
      ssGSM.println((char)26);
  ModuleState=true;
}


void sendHttpPost() {
  String mjson = JSONalarmPosition();
  Serial.println(mjson);
  ssGSM.println("AT+HTTPPOST=\"http://51.68.125.223:3000/api\",\"text/plain\",\"{" + mjson + "}\"");
  delay(1000);
}

void getMobileInfos() {
  ssGSM.flush();
  connectGSM("AT", "OK");
  ssGSM.println("AT+GSN");
  String response = "";
  long int time = millis();
  while ((time + 1000) > millis())
  {
    while (ssGSM.available())
    {
      char c = ssGSM.read();
      response += c;
    }
  }
  //867959034777960
  gsn = response.substring(10, 25);
  Serial.println(gsn);

  ssGSM.println("AT+CIMI");
  response = "";
  time = millis();
  while ((time + 1000) > millis())
  {
    while (ssGSM.available())
    {
      char c = ssGSM.read();
      response += c;
    }
  }
  //208103089430278
  cimi = response.substring(11, 26);
  Serial.println(cimi);
  connectGSM("AT+GPSRD=10", "OK");
}

void setAlarm(String mdata) {
  //ModuleState=false;
  splitstringwithspaces(mdata);
  if (mdata.indexOf("on") >= 0) {
    allat = lat;
    allng = lng;
    algpsdate = gpsdate;
    alflat = flat;
    alflon = flon;
    isAlarm = 1;
    alroll = String(Roll, 3);
    alpitch = String(Pitch, 3);
    alyaw = String(Yaw, 3);
    writeData(makeData());
    sendSmsToAll(alarmPosition());
    }
  else if (mdata.indexOf("off") >= 0) {
    isAlarm = 0;
    sendSmsToAll("ALARM OFF");
    }
  sendHttpPost();
  //ModuleState=true;
  isRemote = 0;
}

void splitstringwithspaces(String message) {

  char str[message.length() + 1];
  char * pch;
  int cnt = 0;

  strcpy(str, message.c_str());
  pch = strtok(str, "\r\n");
  while (pch != NULL) {
    if (cnt > 5) {
      // TODO: erreur il y a plus de caractères séparateurs que prévu
      return;
    }
    data[cnt] = String(pch);
    cnt++;
    pch = strtok(NULL, "\r\n");
  }

}

void initGSM() {
  connectGSM("AT", "OK");
  connectGSM("ATE1", "OK");
  connectGSM("AT+CPIN?", "READY");
}

void initGPRS() {
  connectGSM("AT+CREG=0", "OK");
  connectGSM("AT+CREG?", "0,1");
  connectGSM("AT+CIPSTATUS?", "IP INITIAL");
  connectGSM("AT+CGDCONT=1,\"IP\",\"SL2SFR\"", "OK");
  connectGSM("AT+CGATT=1", "OK");
  connectGSM("AT+CGACT=1,1", "OK");
  connectGSM("AT+CIPSTART=\"TCP\",\"51.68.125.223\",4205","OK");
  ssGSM.flush();
  yield();
}

//convert degrees to radians
double dtor(double fdegrees) {
  return (fdegrees * PI / 180);
}

//Convert radians to degrees
double rtod(double fradians) {
  return (fradians * 180.0 / PI);
}

double CalcDistance(double lat1, double lon1, double lat2, double lon2) {
  double dlon, dlat, a, c;
  double dist = 0.0;
  dlon = dtor(lon2 - lon1);
  dlat = dtor(lat2 - lat1);
  a = pow(sin(dlat / 2), 2) + cos(dtor(lat1)) * cos(dtor(lat2)) * pow(sin(dlon / 2), 2);
  c = 2 * atan2(sqrt(a), sqrt(1 - a));
  dist = 6378140 * c;  //radius of the earth (6378140 meters) in feet 20925656.2
  return ( (long) dist);
}

String makeInfo() {
  double dst = CalcDistance(alflat, alflon, flat, flon );
  return
    "ALARM POSITION\r\n  Lat: " + allat + "\r\n" +
    "  Lon: " + allng + "\r\n" +
    "*************\r\n" +
    "CURRENT POSITION\r\n  Date: " + gpsdate + "\r\n" +
    "  Lat: " + lat + "\r\n" +
    "  Lng: " + lng + "\r\n" +
    "*************\r\n";
}

String alarmPosition() {
  double dst = CalcDistance(alflat, alflon, flat, flon );
  //Serial.println(dst);
  return
    "ALARM POSITION\r\n  Lat: " + allat + "\r\n" +
    "  Lon: " + allng + "\r\n" +
    "  Dist: " + String(dst, 2) + " m\r\n" +
    "*************";
}

void makeMPUData(){
  Accel_x = JY901.getAccX();
  Accel_y = JY901.getAccY();
  Accel_z = JY901.getAccZ();
  Gyro_x = JY901.getGyroX();
  Gyro_y = JY901.getGyroY();
  Gyro_z = JY901.getGyroZ();
  Mag_x = JY901.getMagX();
  Mag_y = JY901.getMagY();
  Mag_z = JY901.getMagZ();
  Roll = JY901.getRoll();
  Pitch = JY901.getPitch();
  Yaw = JY901.getYaw();
  /*Q0 = JY901.getQ0();
  Q1 = JY901.getQ1();
  Q2 = JY901.getQ2();
  Q3 = JY901.getQ3();*/
  Heading = 180 * atan2(Mag_y,Mag_x)/PI;
  if(Heading < 0)Heading += 360;
}



  String makeData(){
    String valt="+33659401246;"+phone[1]+";"+phone[2]+";"+phone[3]+";"+phone[4]+";"+allat+";"+allng+";"+algpsdate+";"+alroll+";"+alpitch+";"+alyaw+"\r";
    Serial.println(valt);
    return valt;
  }

String JSONalarmPosition() {
  double dst = CalcDistance(alflat, alflon, flat, flon );
  return
    "'Alarm':'" + String(isAlarm) + "'," +
    "'Cimi':'" + cimi + "'," +
    "'Gsn':'" + gsn + "'," +
    "'Date':'" + gpsdate + "'," +
    "'Lat':'" + allat + "'," +
    "'Lon':'" + allng + "'," +
    "'Dist':'" + String(dst, 2) + "'," +
    "'cLat': '" + lat + "'," +
    "'cLng': '" + lng + "'," +
    "'Alt': '" + alts + "'," +
    "'Knh': '" + speedn + "'," +
    "'Kmh': '" + speedk + "'," +
    "'Speed': '" + speedg + "'," +
    "'Course': '" + course + "'," +
    "'CapR': '" + capreel + "'," +
    "'CapV': '" + capvrai + "'," +
    "'Valid': '" + vfix + "'," +
    "'HDOP': '" + hdop + "'," +
    "'System': '" + navs + "'," +
    "'NbSat': '" + nbsat + "'," +
    "'Roll': '" + String(Roll,3) + "'," +
    "'Pitch': '" + String(Pitch,3) + "'," +
    "'Yaw': '" + String(Yaw,3) + "'," +
    "'AccX': '" + String(Accel_x,4) + "'," +
    "'AccY': '" + String(Accel_y,4) + "'," +
    "'AccZ': '" + String(Accel_z,4) + "'," +
    "'GyroX': '" + String(Gyro_x,4) + "'," +
    "'GyroY': '" + String(Gyro_y,4) + "'," +
    "'GyroZ': '" + String(Gyro_z,4) + "'," +
    "'MagX': '" + String(Mag_x,4) + "'," +
    "'MagY': '" + String(Mag_y,4) + "'," +
    "'MagZ': '" + String(Mag_z,4) + "'," +
    "'Heading': '" + String(Heading,2) + "'";
}

String makeGpsData() {
  //navs,nbsat,hdop,vfix
  return
    "CURRENT POSITION\r\n  Date: " + gpsdate + "\r\n" +
    "  Lat: " + lat + "\r\n" +
    "  Lng: " + lng + "\r\n" +
    "  Alt: " + alts + "\r\n" +
    "\r\n" +
    "  Kn/h: " + speedn + "\r\n" +
    "  Km/h: " + speedk + "\r\n" +
    "  Speed: " + speedg + "\r\n" +
    "  Course: " + course + "\r\n" +
    "\r\n" +
    "  Cap R: " + capreel + "\r\n" +
    "  Cap V: " + capvrai + "\r\n" +
    "\r\n" +
    "  Valid: " + vfix + "\r\n" +
    "  HDOP: " + hdop + "\r\n" +
    "  System: " + navs + "\r\n" +
    "  Nb Sat: " + nbsat + "\r\n" +
    "*************";
}

String makeGyroData() {
  return
    "GYRO / COMPASS\r\n  Roll: " + String(Roll,3) + "\r\n" +
    "  Pitch: " + String(Pitch,3) + "\r\n" +
    "  Yaw: "  + String(Yaw,3) + "\r\n" +
    "  Heading: "  + String(Heading,3) + "\r\n" +
    "\r\n" +
    "  Acc X: " + String(Accel_x,4) + "\r\n" +
    "  Acc Y: " + String(Accel_y,4) + "\r\n" +
    "  Acc Z: " + String(Accel_z,4) + "\r\n" +
    "\r\n" +
    "  Gyro X: " + String(Gyro_x,4) + "\r\n" +
    "  Gyro Y: " + String(Gyro_y,4) + "\r\n" +
    "  Gyro Z: " + String(Gyro_z,4) + "\r\n" +
    "\r\n" +
    "  Mag X: " + String(Mag_x,4) + "\r\n" +
    "  Mag Y: " + String(Mag_y,4) + "\r\n" +
    "  Mag Z: " + String(Mag_z,4) + "\r\n";

}

void sendSmsToAllInfo(String message) {

  for (int i = 0; i < 1; i++) {
    if (phone[i].indexOf("+") >= 0) {
      connectGSM("AT+CMGF=1", "OK");
      connectGSM("AT+CMGS=\"" + phone[i] + "\"", "");
      delay(1500);
      connectGSM(alarmPosition(), "");
      delay(100);
      connectGSM(makeGpsData(), "");
      delay(100);
      connectGSM(makeGyroData(), "");
      delay(100);
      ssGSM.println((char)26);
      delay(100);

    }

  }

}

void sendSmsToAll(String message) {
  //sendData("AT+CMGF=1", 1000, DEBUG);
  Serial.println(message);
  for (int i = 0; i < 1; i++) {
    if (phone[i].indexOf("+") >= 0) {
      connectGSM("AT+CMGF=1", "OK");
      connectGSM("AT+CMGS=\"" + phone[i] + "\"", "");
      delay(1500);
      connectGSM(message, "");
      delay(1000);
      ssGSM.println((char)26);
      delay(100);
    }

  }

}



String sendData(String command, const int timeout, boolean debug) {
  String response = "";
  ssGSM.println(command);
  long int time = millis();
  while ((time + timeout) > millis())
  {
    while (ssGSM.available())
    {
      char c = ssGSM.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print("deb: ");
    Serial.println(response);
  }
  return response;
}

void connectGSM (String cmd, char *res) {
  digitalWrite(ONBOARD_LED,HIGH);
  while (1) {
    //Serial.println(cmd);
    ssGSM.println(cmd);
    delay(500);
    while (ssGSM.available() > 0) {
      if (ssGSM.find(res)) {
        digitalWrite(ONBOARD_LED,LOW);
        Serial.println(res);
        //delay(1000);
        return;
      }
      //else Serial.println(ssGSM.readString());
    }
    //digitalWrite(ONBOARD_LED,LOW);
    //delay(1000);
  }
}
