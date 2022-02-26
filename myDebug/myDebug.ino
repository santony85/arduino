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

bool done = false;
#define GPRS_APN    "sl2sfr"  ///< Your provider Access Point Name
#define GPRS_USER   NULL        ///< Your provider APN user (usually not needed)
#define GPRS_PASS   NULL        ///< Your provider APN password (usually not needed)
#define NL  "\n"

#if defined(__AVR__)
    typedef __FlashStringHelper* __StrPtr;
#else
    typedef const char* __StrPtr;
#endif

#define MESSAGE_LENGTH 160
char message[MESSAGE_LENGTH];
int messageIndex = 0;
char MESSAGE[300];
char phone[16]="0659401246";
char datetime[24];

String slat, slon,sspeed,salt,scourse,ssattelites,sutc;

#include <PPPOS.h>
#include <PPPOSClient.h>
#include <PubSubClient.h>

#define SERIAL_BR           115200
#define GSM_SERIAL          2
#define GSM_RX              16
#define GSM_TX              17
#define GSM_BR              9600

char* server = "51.68.125.223";
char* ppp_user = "";
char* ppp_pass = "";
String APN = "sl2sfr";


#define WEB_SERVER "www.w3.org"
#define WEB_URL "https://www.w3.org/TR/PNG/iso_8859-1.txt"
static const char *REQUEST = "GET " WEB_URL "\ HTTP/1.1\r\n"
    "Host: "WEB_SERVER"\r\n"
    "Connection: keep-alive\r\n"
    "User-Agent: esp/1.0 esp32\r\n"
    "\r\n";

String buffer = "";
char *data = (char *) malloc(1024); 
bool atMode = true;


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

PPPOSClient ppposClient;
PubSubClient client(ppposClient);

bool sendCommandWithAnswer(String cmd, String ans){
         PPPOS_write((char *)cmd.c_str());
         unsigned long _tg = millis();
         while(true){
          data = PPPOS_read();
          if (data != NULL){
            char* command = strtok(data, "\n");
            while (command != 0)
            {
              buffer = String(command);
              buffer.replace("\r", "");
              command = strtok(0, "\n");
              if (buffer != "") { Serial.println(buffer); }
              if (buffer == ans) {buffer = ""; return true; }
              buffer = "";
            } 
          }
            if (millis() > (_tg + 5000)) { buffer = ""; return false; } 
         }
         buffer = "";
         return false;
}

bool startPPPOS(){  
      String apnSet = "AT+CGDCONT=1,\"IP\",\"" + APN + "\"\n";
      if (!sendCommandWithAnswer(apnSet, "OK")) { return false; }
      if (!sendCommandWithAnswer("AT+CGDATA=\"PPP\",1\n", "CONNECT")) { return false; }
      atMode = false;
      PPPOS_start(); 
      unsigned long _tg = millis();
      while(!PPPOS_isConnected()) {
        if (millis() > (_tg + 10000)) { PPPOS_stop();  atMode = true; return false; }
      }
      Serial.println("PPPOS Started");
      return true;
}

bool enterATModePPPOS(){
      if (PPPOS_isConnected() && !atMode){
        if (sendCommandWithAnswer("+++", "OK")) { atMode = true; return true;}
      }
      return false;
}

bool cancelATModePPPOS(){
      if (PPPOS_isConnected() && atMode){
        if (sendCommandWithAnswer("ATO\n", "CONNECT")) { atMode = false; return true;}
      }
      return false;
}

void simpleGetRequest(){
  if (!ppposClient.connected() ) {
          Serial.println("Connecting...");
           ppposClient.connect(WEB_SERVER, 80);
  }
  if (ppposClient.connected() ) {
           Serial.println("Connected");
           Serial.println(ppposClient.write(REQUEST, strlen(REQUEST)));
           while(ppposClient.available()){
            Serial.print((char)ppposClient.read());
           }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

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
    delay(100);
    sim808.print("AT+CMGF=1\r");
    delay(400);
    sim808.print("ATS0=1\r");
    delay(100);

    PPPOS_init(GSM_TX, GSM_RX, GSM_BR, GSM_SERIAL, ppp_user, ppp_pass);
    client.setServer(server, 1883);
    client.setCallback(callback);
  
}

void startGPRS(){
    SIM808NetworkRegistrationState status = sim808.getNetworkRegistrationStatus();
    SIM808SignalQualityReport report = sim808.getSignalQuality();
    bool isAvailable = static_cast<int8_t>(status) &
        (static_cast<int8_t>(SIM808NetworkRegistrationState::Registered) | static_cast<int8_t>(SIM808NetworkRegistrationState::Roaming))
        != 0;

    if(!isAvailable) {
        Log.notice(S_F("No network yet..." NL));
        delay(1000);
        return;
    }

    Log.notice(S_F("Network is ready." NL));
    Log.notice(S_F("Attenuation : %d dBm, Estimated quality : %d" NL), report.attenuation, report.rssi);

    bool enabled = false;
    do {
        Log.notice(S_F("Powering on SIM808's GPRS..." NL));
        enabled = sim808.enableGprs(GPRS_APN, GPRS_USER, GPRS_PASS);        
    } while(!enabled);
    Log.notice(S_F("GPRS Ready" NL));
    String isTcp="";
    do{
      isTcp = sim808.enableTcp("51.68.125.223","1883", GPRS_APN);
    }while(isTcp == "");
    
    Log.notice(S_F("TCP Ready" NL));
    done = true;
}

void loop(){
  if(!done){
    //startGPRS();
    if (startPPPOS()) { 
      Serial.println("Starting PPPOS... OK"); 
      done = true;
      } 
    else { 
      Serial.println("Starting PPPOS... Failed"); 
    }

  }
  else{
    handleGPS();
    handleSMS();
  }

  // ------- for debug only ----- 
  while(sim808.available()){
    Serial.println(sim808.readString());
    }
  while(Serial.available()){
    String dt = Serial.readString();
    Serial.println(dt);
    if(dt.indexOf("#a")>-1){
      String msg = sim808.readSms(5);
      Serial.println(msg);
    }
   else if(dt.indexOf("#d")>-1){
      sim808.deleteAllSms();
    }
    else if(dt.indexOf("#s")>-1){
      sendSMS();
    }
    else {
      sim808.println(dt);
      Serial.println(dt);
      }
    }
  ///---------------------
   
  //delay(3000);
}
void sendSMS(){
  bool success = sim808.sendSms("+33659401246", makeGpsDataSms().c_str());  
}

void handleSMS(){
  char lstsms[3];
  int idSMS = sim808.isSMSunread(lstsms,3);
  if(idSMS >0){
    String msg = sim808.readSms(idSMS);
    if(msg.indexOf("#info")>-1){
      Serial.println("info");
      sendSMS(); 
    }
    sim808.deleteSms(idSMS);
  }
  //delay(3000);
}

String parseUtc(char * timeStr){
  String mdata = timeStr;
  String _year = mdata.substring(0,4);
  String _month = mdata.substring(4,6);
  String _day = mdata.substring(6,8);
  String _hour =   mdata.substring(8,10);
  String _minute = mdata.substring(10,12);
  String _second = mdata.substring(12,14);
  String mtime = _hour+":"+_minute+":"+_second;
  return _year+"-"+_month+"-"+_day+" "+mtime; 
}

String makeGpsDataSms() {
  //navs,nbsat,hdop,vfix
  return
    "CURRENT POSITION\r\n  Date: " + sutc + "\r\n" +
    "  Lat: " + slat + "\r\n" +
    "  Lng: " + slon + "\r\n" +
    "  Alt: " + salt + "\r\n" +
    "  Km/h: " + sspeed + "\r\n" +
    "  Course: " + scourse + "\r\n" +
    "  Nb Sat: " + ssattelites + "\r\n" +
    "*************";
}

void handleGPS(){
    SIM808GpsStatus status = sim808.getGpsStatus(position, POSITION_SIZE);
    if(status < SIM808GpsStatus::Fix) {
        Log.notice(S_F("No fix yet..." NL));
        delay(NO_FIX_GPS_DELAY);
        return;
    }

    uint16_t sattelites;
    char * timeStr;
    float lat, lon,speed,alt,course;
    //20220224142741
    __StrPtr state;

    if(status == SIM808GpsStatus::Fix) state = S_F("Normal");
    else state = S_F("Accurate");
    sim808.getGpsField(position, SIM808GpsField::Utc, &timeStr);
    sim808.getGpsField(position, SIM808GpsField::GnssUsed, &sattelites);
    sim808.getGpsField(position, SIM808GpsField::Latitude, &lat);
    sim808.getGpsField(position, SIM808GpsField::Longitude, &lon);
    sim808.getGpsField(position, SIM808GpsField::Speed, &speed);
    sim808.getGpsField(position, SIM808GpsField::Altitude, &alt);
    sim808.getGpsField(position, SIM808GpsField::Course, &course);
    
    sutc=parseUtc(timeStr);
    ssattelites=String(sattelites);
    slat=String(lat,6);
    slon=String(lon,6);
    sspeed=String(speed,2);
    salt=String(alt,2);
    scourse=String(course,2);
    //Serial.println(sutc);
    //Serial.println(ssattelites);
    /*Serial.println(slat);
    Serial.println(slon);
    Serial.println(sspeed);
    Serial.println(salt);
    Serial.println(scourse);*/

    //Serial.println(position);    
    delay(FIX_GPS_DELAY);
}
