bool ppsTriggered = false;
String capreel="0.0",capvrai="0.0",speedn="",speedk="",speedg="",course="",navs,nbsat,hdop="0.0",valid;
String lat = "51.508131", lng = "-1.128002", alts = "0.0", gpsdate = "10:00:00", speed = "0.00";

String allat = "51.508131", allng = "-1.128002", algpsdate = "10:00:00";
double flat, flon, falt, alflat = 51.508131, alflon = -1.128002, fspeed=0.0;

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
  //TelnetStream.println(gdata);
  //$GNVTG,0.00,T,,M,0.000,N,0.000,K,N*2C
  capreel=getValueGPS(gdata,',',1);
  capvrai=getValueGPS(gdata,',',3);
  speedn= getValueGPS(gdata,',',5);
  speedk= getValueGPS(gdata,',',7);
}

void parseGNGGA(String gdata){
  
  if(gdata.indexOf("$GNGGA")>-1 && gdata.indexOf("*")>-1){
    TelnetStream.println(gdata);
 
  //parse lat
  String _latitude = getValueGPS(gdata,',',2);
  String _latdir = getValueGPS(gdata,',',3);
  if(_latdir =="N"){
    flat = _latitude.toFloat() / 100.0;
    lat = String(flat,6);
    //Serial.println(lat);
    //TelnetStream.println(lat);     
    }
  else if(_latdir =="S"){
    flat = 0.0 - (_latitude.toFloat() / 100.0);
    lat = String(flat,6);
    //Serial.println(lat);
    //TelnetStream.println(lat);   
    }
  //parse lng
  String _longitude = getValueGPS(gdata,',',4);
  String _lngdir = getValueGPS(gdata,',',5);
  if(_lngdir =="E"){
    flon = _longitude.toFloat() / 100.0;
    lng = String(flon,6);
    //Serial.println(lng);
    //TelnetStream.println(lng);     
    }
  else if(_lngdir =="W"){
    flon = 0.0 - (_longitude.toFloat() / 100.0);
    lng = String(flon,6);
    //Serial.println(lng);
    //TelnetStream.println(lng);   
    }
  valid=getValueGPS(gdata,',',6);
  nbsat=getValueGPS(gdata,',',7);
  hdop =getValueGPS(gdata,',',8);
  alts=getValueGPS(gdata,',',9);
  }
}

unsigned int parseUnsignedInt(const char *s, uint8_t len)
{
  int r = 0;
  while (len--)
    r = 10 * r + *s++ - '0';
  return r;
}

void parseGMRC(String gdata){
  //$GNRMC,234445.000,A,4629.7878,N,00146.5601,W,0.446,153.73,150222,,,A*59
  String mdata = getValueGPS(gdata,',',1);
  String _hour =   mdata.substring(0,2);
  String _minute = mdata.substring(2,4);
  String _second = mdata.substring(4,6);
  String mtime = _hour+":"+_minute+":"+_second;
  //Serial.println(mtime);//ok
  mdata= getValueGPS(gdata,',',9);
  String _day = mdata.substring(0,2);
  String _month = mdata.substring(2,4);
  String _year = mdata.substring(4,6);
  gpsdate = "20"+_year+"-"+_month+"-"+_day+" "+mtime;
  //Serial.println(gpsdate);
  
}

String makeGpsDataSms() {
  //navs,nbsat,hdop,vfix
  return
    "CURRENT POSITION\r\n  Date: " + gpsdate + "\r\n" +
    "  Lat: " + lat + "\r\n" +
    "  Lng: " + lng + "\r\n" +
    "  Alt: " + alts + "\r\n" +
    "\r\n" +
    "  Kn/h: " + speedn + "\r\n" +
    "  Km/h: " + speedk + "\r\n" +
    //"  Speed: " + speedg + "\r\n" +
    //"  Course: " + course + "\r\n" +
    "\r\n" +
    "  Cap R: " + capreel + "\r\n" +
    "  Cap V: " + capvrai + "\r\n" +
    "\r\n" +
    "  HDOP: " + hdop + "\r\n" +
    //"  System: " + navs + "\r\n" +
    "  Nb Sat: " + nbsat + "\r\n" +
    "*************";
}

String makeGpsDataJson() {
  //navs,nbsat,hdop,vfix
  return
    "{'table':{'name':'globe','Date': '" + gpsdate + "'," +
    "'Lat': '" + lat + "'," +
    "'Lng': '" + lng + "'," +
    "'Alt': '" + alts + "'," +
    "'Knh': '" + speedn + "'," +
    "'Kmh': '" + speedk + "'," +
    //"'Speed': '" + speedg + "'," +
    //"'Course': '" + course + "'," +
    "'CapR': '" + capreel + "'," +
    "'CapV': '" + capvrai + "'," +
    "'HDOP': '" + hdop + "'," +
    //"'System': '" + navs + "'," +
    "'NbSat': '" + nbsat + "'}},";
}

void buildtrame(String igdata){
  String gdata=igdata;
  int num =countLines(gdata,'\r');
  parseGNGGA(getValueGPS(gdata,'\r',0));
  parseVTG(getValueGPS(gdata,'\r',num-1));
  parseGMRC(getValueGPS(gdata,'\r',num-2));
  //makeGPS();
  //Serial.println(makeGpsDataJson()); 
  //TelnetStream.println(makeGpsDataJson());
  //if(isListenGPS)connectGSM("AT+MQTTPUB=\""+mname+"\",\""+makeGpsDataJson()+"\",0,0,0","",false); 

}
