
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>
#include <JY901.h>


#include "LittleFS.h"

#include <MicroNMEA.h>

#define MAX_LEN_MESSAGE 20000 
#define DEBUG true

SoftwareSerial ssGSM(14, 16);
SoftwareSerial ssMPU(12, 13);


int RST_KEY = 15;


int isInit=0;
String msgIn;
char* command;
//String mdata;
String data[10]; 

bool ModuleState=true;
String phone[5]={"+33659401246"," "," "," "," "};

//String ALARM_LAT = "51.508131", ALARM_LON = "-1.128002", ALARM_TIME="0";
int isAlarm=0;

String lat="51.508131",lng="-1.128002",alts="",gpsdate="10:00:00";
String allat="51.508131",allng="-1.128002",algpsdate="10:00:00";
long alt;

double flat,flon,alflat=51.508131,alflon=-1.128002;

String myIp;
int isStart=0;


//MicroNMEA library structures
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));




void setup() {
  // put your setup code here, to run once:

    pinMode(RST_KEY, OUTPUT);
    pinMode(BUILTIN_LED, OUTPUT); 
    digitalWrite(BUILTIN_LED, LOW); 
    Serial.begin(115200);
    ssGSM.begin(115200);
    ssMPU.begin(9600);
    delay(500);
    Serial.println();
    Serial.println();
    Serial.println("Starting");

    
    digitalWrite(RST_KEY, LOW);
    delay(200); 
    digitalWrite(RST_KEY, HIGH); 
    if(!LittleFS.begin()){
     Serial.println("An Error has occurred while mounting LittleFS");
     delay(1000);
     return;
    }
    File file = LittleFS.open("/numero.txt", "r");
    if(!file){
        Serial.println("No Saved Data!");
        writeData(makeData(),"/numero.txt");
      }
    readData();

   file = LittleFS.open("/log.txt", "r");
      if(!file){
        Serial.println("No Saved Data!");
        writeData("Init\n","/log.txt");
      }

   //modulePowerOn();
   delay(1000);
   sendData("AT+IPR=115200", 5000, DEBUG);
   delay(5000);
   //JY901.attach(ssMPU);     
   Serial.println("ready to go"); 
}


void readData(){
  File file = LittleFS.open("/numero.txt", "r");
   String val="";
   while(file.available()){
      char xx = (char)file.read();
      val.concat(xx);
   }
   file.close();
   char buf[1024];
   val.toCharArray(buf, sizeof(buf));
   char *p = buf;
   char *str;
   int a=0;
   while ((str = strtok_r(p, ";", &p)) != NULL){ // delimiter is the semicolon
      if(a<5)phone[a++]=str;
    }
  }

String makeData(){
  /*for(int i=0;i<10;i++){
    Serial.print(i);
    Serial.print("->");
    Serial.println(phone[i]);
  }*/
  String valt="+33659401246;"+phone[1]+";"+phone[2]+";"+phone[3]+";"+phone[4]+";"+allat+";"+allng+";"+algpsdate;
  Serial.println(valt);
  return valt;
  }

void writeData(String datar,String fname){
  File file = LittleFS.open(fname, "w");
  file.print(datar);
  delay(1);
  file.close();
  }

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  // put your main code here, to run repeatedly:
  //JY901.receiveSerialData();  
  //mode console
  if(ssMPU.available() > 0) {
    Serial.println(ssMPU.readString());
    }
  
  if(Serial.available() > 0) {
    msgIn = Serial.readString();
    Serial.println(msgIn);
    if(msgIn =="1"){
      Serial.println("set SMS mode");
      sendData("AT+CPMS=\"SM\",\"SM\",\"SM\"", 1000,DEBUG);
      sendData("AT+CMGF=1", 1000, DEBUG);
      sendData("AT+CNMI=2,2,0,0,0", 1000, DEBUG);
      sendData("AT+GPS=1", 1000, DEBUG);
      yield();
    }
    else if(msgIn =="2"){
      Serial.println("send SMS");
      sendData("AT+CMGF=1", 1000, DEBUG);
      sendData("AT+CMGS=\"+33659401246\"", 1000, DEBUG);
      ssGSM.print("Le message");
      delay(100);
      ssGSM.println((char)26);
      yield();
    }
    else if(msgIn =="3"){
      sendData("ATD"+phone[0]+";", 1000, DEBUG);
    }
    else if(msgIn =="4"){
      sendData("AT+CPMS=\"ME\"", 2000,DEBUG);
      sendData("AT+CMGD=1,4", 5000, DEBUG);
      sendData("AT+CMGF=1", 1000, DEBUG);
    }
    else if(msgIn =="5"){ 
      ModuleState=false;
      sendData("AT+CGATT=1,1",10000,DEBUG);
      delay(10000); 
      String msg =sendData("AT+HTTPGET=\"http://51.68.125.223:3000\"",5000,DEBUG); 
      delay(100);
      ssGSM.println((char)26);
      Serial.println(msg);
      ModuleState=true;
      }
    else {
      Serial.println("other");
      String msg = String("");
      msg = sendData(msgIn, 1000, DEBUG);
      Serial.println(msg);
    }
  }


  //data GSM SMS IN
  if(ssGSM.available() > 0 && ModuleState) {
    String mdata = ssGSM.readString();
    if(mdata.indexOf("#info") >= 0){
      ModuleState=false;
      //sendData("AT+GPS=0", 1000, DEBUG);
      sendData("AT+GPSRD=0", 1000, DEBUG);
      sendSmsToAll(alarmPosition()+"-------------\r\n"+makeGpsData()+"-------------\r\n"+makeGyroData());
      sendData("AT+GPSRD=10", 1000, DEBUG);
      ModuleState=true;
      }
    /*else if(mdata.indexOf("#set-number") >= 0){
          Serial.println(data[3]);
          Serial.println(data[2]);
          phone[data[3].toInt()]=data[2];
          writeData(makeData(),"/numero.txt");
          sendSmsToAll("Number added successfuly");
         }*/
         
   //call numbers  
   else if(mdata.indexOf("#call") >= 0){
      sendData("ATD"+phone[0]+";", 1000, DEBUG); 
      }
    //set alarm  
    else if(mdata.indexOf("#alarm") >= 0){
         ModuleState=false;
         //sendData("AT+GPS=0", 1000, DEBUG);
         sendData("AT+GPSRD=0", 1000, DEBUG);
         splitstringwithspaces(mdata);
         Serial.println(data[2]);
         if(data[2].indexOf("on") >= 0){
             //setting position
             allat=lat;
             allng=lng;
             algpsdate=gpsdate;
             alflat=flat;
             alflon=flon;
             writeData(makeData(),"/numero.txt"); 
             isAlarm=1;
             sendSmsToAll(alarmPosition());
          }
         else if(data[2].indexOf("off") >= 0){
             isAlarm=0;
             sendSmsToAll("ALARM OFF");
          }
         //sendData("AT+GPS=1", 1000, DEBUG);
         sendData("AT+GPSRD=10", 1000, DEBUG);
         ModuleState=true;
         } 
    //get gps data     
    else if(mdata.indexOf("+GPSRD:$GNGGA") >= 0){
      digitalWrite(BUILTIN_LED, LOW); 
      //ModuleState=false;
      int posCR = mdata.indexOf("\r");
      String gngga = mdata.substring(7,posCR);
      int str_len = gngga.length() + 1;
      gngga.toCharArray(nmeaBuffer, str_len);
      Serial.println(gngga);
      nmea.processGGA(nmeaBuffer); 
      flat = nmea.getLatitude()/ 1000000.;
      flon = nmea.getLongitude()/ 1000000.;
      lat = String(flat,6);
      lng = String(flon,6);
      alts = String(nmea.getAltitude(alt)/ 1000.,3);
      gpsdate=getValue(gngga,',',1);
      String dts = gpsdate.substring(0,2)+":"+gpsdate.substring(2,4)+":"+gpsdate.substring(4,6);
      gpsdate=dts;
      Serial.println(gpsdate);
      //ModuleState=true;
      if(isAlarm){
        double dst = CalcDistance(alflat, alflon, flat, flon );
        if(dst > 5){
          Serial.print(dst);
          Serial.println(" m DANGER !!!");
          //sendSmsToAll("DANGER !!!!");
          }
        }
        digitalWrite(BUILTIN_LED, HIGH);
      }
    //wait ready
    else if(mdata.indexOf("+CREG: 1") >= 0){
      sendData("AT+CPMS=\"ME\"", 2000,DEBUG);
      sendData("AT+CMGD=1,4", 5000, DEBUG);
      sendData("AT+CMGF=1", 1000, DEBUG);
      //sendData("AT+CNMI=2,2,0,0,0", 1000, DEBUG);
      //sendData("AT+CREG?",3000,DEBUG); 
      //sendData("AT+CGREG?",3000,DEBUG);
      sendData("AT+CGATT=1",1000,DEBUG); 
      delay(1000);
      sendData("AT+CGDCONT=1,\"IP\",\"SL2SFR\",0,0",2000,DEBUG);
      delay(10000);
      sendData("AT+GPS=1", 1000, DEBUG);
      sendData("AT+GPSRD=10", 1000, DEBUG);
      }
    else {
      Serial.print("ret: ");
      Serial.println(mdata);
    }
    //yield();
  }
  //AI GPS GYRO
  
  //smartdelay(0);
}
//convert degrees to radians
double dtor(double fdegrees){
  return(fdegrees * PI / 180);
  }

//Convert radians to degrees
double rtod(double fradians){
  return(fradians * 180.0 / PI);
  }

double CalcDistance(double lat1, double lon1, double lat2, double lon2){
  double dlon, dlat, a, c;
  double dist = 0.0;
  dlon = dtor(lon2 - lon1);
  dlat = dtor(lat2 - lat1);
  a = pow(sin(dlat/2),2) + cos(dtor(lat1)) * cos(dtor(lat2)) * pow(sin(dlon/2),2);
  c = 2 * atan2(sqrt(a), sqrt(1-a));
  dist = 6378140 * c;  //radius of the earth (6378140 meters) in feet 20925656.2
  return( (long) dist);
}

void sendSmsToAll(String message){
  //sendData("AT+CMGF=1", 1000, DEBUG);
  for(int i=0;i<1;i++){
    if(phone[i].indexOf("+") >= 0){
      Serial.print(i);
      Serial.print("->");
      Serial.println(phone[i]);
      String msg = String("");
      sendData("AT+CMGF=1", 1000, false);
      sendData("AT+CMGS=\""+phone[i]+"\"", 1000, false);
      msg = sendData(message, 5000, false);
      //ssGSM.print(message);
      delay(100);
      ssGSM.println((char)26);
      yield();
      } 
  }
  
}

String alarmPosition(){
  double dst = CalcDistance(alflat, alflon, flat, flon );
  //Serial.println(dst);
  return 
    "ALARM POSITION\r\n  Lat: "+allat+"\r\n"+
    "  Lon: "+allng+"\r\n"+
    "  Dist: "+dst+" m\r\n";
  
}

String makeGpsData(){
return
    "CURRENT POSITION\r\n  Date: "+gpsdate+"\r\n"+ 
    "  Lat: "+lat+"\r\n"+
    "  Lng: "+lng+"\r\n"+
    "  Alt: "+alts+"\r\n";
    //"  Speed: "+String(gps.f_speed_knots())+"\r\n";
}

String makeGyroData(){
  return

    "GYRO / COMPASS\r\n  Roll: "+String(JY901.getRoll())+"\r\n"+
    "  Pitch: "+String(JY901.getPitch())+"\r\n"+
    "  Yaw: "+String(JY901.getYaw())+"\r\n"+
    "\r\n"+
    "  Acc X: "+String(JY901.getAccX())+"\r\n"+
    "  Acc Y: "+String(JY901.getAccY())+"\r\n"+
    "  Acc Z: "+String(JY901.getAccZ())+"\r\n"+
    "\r\n"+
    "  Gyro X: "+String(JY901.getGyroX())+"\r\n"+
    "  Gyro Y: "+String(JY901.getGyroY())+"\r\n"+
    "  Gyro Z: "+String(JY901.getGyroZ())+"\r\n"+
    "\r\n"+
    "  Mag X: "+String(JY901.getMagX())+"\r\n"+
    "  Mag Y: "+String(JY901.getMagY())+"\r\n"+
    "  Mag Z: "+String(JY901.getMagZ())+"\r\n";

}

void splitstringwithspaces(String message) {
    if (message.length() > MAX_LEN_MESSAGE) {
        // TODO: erreur message trop long
        return;
    } else {
        char str[MAX_LEN_MESSAGE + 1];
        char * pch;
        int cnt = 0;

        strcpy(str, message.c_str());
        pch = strtok(str,"\r\n");
        while (pch != NULL) {
            if (cnt > 5) {
                // TODO: erreur il y a plus de caractères séparateurs que prévu
                return;
            }
            data[cnt] = String(pch);
            cnt++;
            pch = strtok(NULL,"\r\n");
        }
    }
}



String sendData(String command, const int timeout, boolean debug){
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
