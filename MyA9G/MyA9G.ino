#include <SoftwareSerial.h>
SoftwareSerial ssGSM(5, 4);
#include <TinyGPS.h>
TinyGPS gps;
//SoftwareSerial ssGPS(14, 16);

String msgIn;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  Serial.println("Starting");
  ssGSM.begin(115200);
  //ssGPS.begin(115200);
  delay(2000);
  //sendData("AT+GPS=1",2000);
  //sendData("AT+RST=1",3000);
  sendData("AT+CPMS=\"SM\",\"SM\",\"SM\"", 1000);
  sendData("AT+CMGF=1", 1000);

}

void loop() {
  String response = "";
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0) {
    msgIn = Serial.readString();
    //Serial.println(msgIn);
    //ssGSM.println(msgIn);
    //sendData(msgIn,2000);
    sendData("AT+CMGF=1", 1000);
    sendData("AT+CMGS=\"+33659401246\"", 2000);
    delay(5000);
    ssGSM.print("Le message");
    delay(100);
    ssGSM.println((char)26);
    yield();
  }

if(ssGSM.available() > 0) {
   String mdata = ssGSM.readString();
    //if(mdata.indexOf("+CMT:") >=0){
      //Serial.println(mdata);
    //  }
}
  
  /*if(ssGPS.available() > 0){
    Serial.print(ssGPS.read());   
  }*/
//Serial.println(makeGpsData());
delay(1000);

}

String makeGpsData(){
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  return
    "CURRENT POSITION\r\n  Date: "+String(print_date(gps))+"\r\n"+ 
    "  Lat: "+String(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6)+"\r\n"+
    "  Lng: "+String(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6)+"\r\n"+
    "  Alt: "+String(gps.f_altitude())+"\r\n"+
    "  Speed: "+String(gps.f_speed_knots())+"\r\n";
    //"Dist: "+String(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long)TinyGPS::distance_between(flat, flon, ALARM_LAT, ALARM_LON) / 1000, 0xFFFFFFFF, 9)+"\r\n";
}

String print_date(TinyGPS &gps){
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
    return sz;

  }
  //print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  //smartdelay(0);
}

String sendData(String command, const int timeout){
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

        Serial.println(response);

    return response;
}
