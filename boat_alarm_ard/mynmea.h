bool ppsTriggered = false;
bool isValid=false;
String capreel,capvrai,speedn,speedk,speedg,course,navs,nbsat,hdop,vfix;

void printGPS(){
  //Serial.println(nmea.isValid());
  Serial.println(nmea.getLatitude());
  isValid = nmea.isValid();
  if(!nmea.isValid()){
    vfix = nmea.isValid() ? "yes" : "no";
    if (nmea.getNavSystem()){
      navs =nmea.getNavSystem(); 
    }
      
    else{
      navs="none";
    }
      
    nbsat=String(nmea.getNumSatellites());
    //Serial.print("HDOP: ");
    hdop=String(nmea.getHDOP()/10., 1);
    String gpdate = String(nmea.getYear())+'-'+String(nmea.getMonth() < 10 ? "0"+ String(nmea.getMonth()) : String(nmea.getMonth()))+'-'+String(nmea.getDay() < 10 ? "0"+String(nmea.getDay()) : String(nmea.getDay()));
    gpdate = gpdate+" "+String(int(nmea.getHour()+1))+":"+String( nmea.getMinute() < 10 ? "0"+String(nmea.getMinute()) : String(nmea.getMinute()) )+":"+String(nmea.getSecond() < 10 ? "0"+String(nmea.getSecond()) : String(nmea.getSecond()));
    gpsdate = gpdate;
    Serial.println(gpsdate);
    long latitude_mdeg = nmea.getLatitude();
    long longitude_mdeg = nmea.getLongitude();
    flat = nmea.getLatitude()/ 1000000.;
    flon = nmea.getLongitude()/ 1000000.;
    lat = String(flat,6);
    lng = String(flon,6);
    
    long alt;
    if (nmea.getAltitude(alt)){
      falt = alt/ 1000.;
      alts= String(falt, 3);
      } 
    else{
      falt = 0.0;
      alts="n/a";
      }
    speedg = String(nmea.getSpeed() / 1000.,3);
    course = String(nmea.getCourse() / 1000.,3);

  #ifdef MEMORY_FREE_H
    Serial.print("freeMemory()=");
    Serial.println(freeMemory());
  #endif
    nmea.clear();
  }  
}

String getValueGPS(String data, char separator, int index){
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

int countLines(String vin, char separator){
  int maxIndex = vin.length();
  int cnt=0;
  for(int i=0;i<maxIndex;i++){
    if(vin.charAt(i)==separator)cnt++;
  }
  return cnt;
}

void parseVTG(String gdata){
  //Serial.println(gdata);
  //$GNVTG,0.00,T,,M,0.000,N,0.000,K,N*2C
  capreel=getValueGPS(gdata,',',1);
  capvrai=getValueGPS(gdata,',',3);
  speedn= getValueGPS(gdata,',',5);
  speedk= getValueGPS(gdata,',',7);
  /*Serial.println(capreel);
  Serial.println(capvrai);
  Serial.println(speedn);
  Serial.println(speedk);*/
}

void parseGGNA(String gdata){
  //Serial.println(gdata);
  //$GNGGA,202508.000,4629.7823,N,00146.5467,W,1,6,1.14,64.5,M,49.1,M,,*60
  nbsat=getValueGPS(gdata,',',6);
  //Serial.println(nbsat);
  alts=getValueGPS(gdata,',',9);
  //Serial.println(alts);
}
void parseGMRC(String gdata){
  //$GNRMC,215923.000,A,4629.7903,N,00146.5614,W,0.187,241.45,100122,,,A*55
  //Serial.println(gdata);
  }


void buildtrame(String gdata){
  int num =countLines(gdata,'\r');
  String GMRC= getValueGPS(gdata,'\r',num-2);
  parseGMRC(GMRC);
  char out_str[GMRC.length()]; 
  strcpy(out_str, GMRC.c_str());
  for(int i=0;i<GMRC.length();i++){
    char c = out_str[i];
    nmea.process(c); 
  }
  printGPS();
  parseVTG(getValueGPS(gdata,'\r',num-1));
  parseGGNA(getValueGPS(gdata,'\r',0));
   
}
