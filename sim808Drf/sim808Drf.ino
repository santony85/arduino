#include <DFRobot_SIM808.h>
#include <SoftwareSerial.h>

#define MESSAGE_LENGTH 160
char message[MESSAGE_LENGTH];
int messageIndex = 0;
char MESSAGE[300];
char lat[12];
char lon[12];
char wspeed[12];

char phone[16]="0659401246";
char datetime[24];


char gprsBuffer[64];
char *s = NULL;

#define PIN_TX 16
#define PIN_RX 17
SoftwareSerial mySerial(PIN_TX,PIN_RX);
DFRobot_SIM808 sim808(&mySerial);//Connect RX,TX,PWR,
int RST_KEY = 15;
int isInitTCP=0;
void sendSMS();
void getGPS();
void readSMS();


void setup()
{
  pinMode(RST_KEY, OUTPUT); 
  mySerial.begin(9600);
  Serial.begin(115200);
  Serial.println("Start");
  //******** Initialize sim808 module *************
  digitalWrite(RST_KEY, LOW);
  delay(200);
  digitalWrite(RST_KEY, HIGH);
  delay(500);
  while(!sim808.init())
  {
      Serial.print("Sim808 init error\r\n");
      delay(1000);
  }
  
  while(!sim808.attachGPS())
  {
    Serial.println("Open the GPS power failure");
    delay(1000);
  }
  delay(3000);
 
}

void loop()
{
  
   
  //*********** Detecting unread SMS ************************
   messageIndex = sim808.isSMSunread();
   
   //*********** At least, there is one UNREAD SMS ***********
   if (messageIndex > 0)
   { 
    if(sim808.readable()){
      sim808_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
      Serial.println(gprsBuffer);
      if(String(gprsBuffer).indexOf("RING"))sim808.answer();
      sim808_clean_buffer(gprsBuffer,32);
      }  
      //if(!isInitTCP)initTCP();
      readSMS();
      getGPS();
      //sendSMS();
      //************* Turn off the GPS power ************
      //sim808.detachGPS();
   }
   //getGPS();
}

void readSMS()
{
  Serial.print("messageIndex: ");
  Serial.println(messageIndex);
  sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
             
  //***********In order not to full SIM Memory, is better to delete it**********
  sim808.deleteSMS(messageIndex);
  Serial.print("From number: ");
  Serial.println(phone);  
  Serial.print("Datetime: ");
  Serial.println(datetime);        
  Serial.print("Recieved Message: ");
  Serial.println(message);
  if(String(message).indexOf("#info")>-1 && messageIndex < 255){
    sendSMS();
  }
}

void getGPS()
{ 
  while(!sim808.attachGPS())
  {
    Serial.println("Open the GPS power failure");
    delay(1000);
  }
  delay(3000);

  //Serial.println("Open the GPS power success");
    
  while(!sim808.getGPS())
  {
    
  }

  Serial.print(sim808.GPSdata.year);
  Serial.print("/");
  Serial.print(sim808.GPSdata.month);
  Serial.print("/");
  Serial.print(sim808.GPSdata.day);
  Serial.print(" ");
  Serial.print(sim808.GPSdata.hour);
  Serial.print(":");
  Serial.print(sim808.GPSdata.minute);
  Serial.print(":");
  Serial.print(sim808.GPSdata.second);
  Serial.print(":");
  Serial.println(sim808.GPSdata.centisecond);
  Serial.print("latitude :");
  Serial.println(sim808.GPSdata.lat);
  Serial.print("longitude :");
  Serial.println(sim808.GPSdata.lon);
  Serial.print("speed_kph :");
  Serial.println(sim808.GPSdata.speed_kph);
  Serial.print("heading :");
  Serial.println(sim808.GPSdata.heading);
  Serial.println();


  float la = sim808.GPSdata.lat;
  float lo = sim808.GPSdata.lon;
  float ws = sim808.GPSdata.speed_kph;

  dtostrf(la, 4, 6, lat); //put float value of la into char array of lat. 4 = number of digits before decimal sign. 6 = number of digits after the decimal sign.
  dtostrf(lo, 4, 6, lon); //put float value of lo into char array of lon
  dtostrf(ws, 6, 2, wspeed);  //put float value of ws into char array of wspeed

  sprintf(MESSAGE, "Latitude : %s\nLongitude : %s\nWind Speed : %s kph\nMy Module Is Working. Mewan Indula Pathirage. Try With This Link.\nhttp://www.latlong.net/Show-Latitude-Longitude.html\nhttp://maps.google.com/maps?q=%s,%s\n", lat, lon, wspeed, lat, lon);
}

void initTCP(){
   //*********** Attempt DHCP *******************
    while(!sim808.join(F("sl2sfr"))) {
        Serial.println("Sim808 join network error");
        delay(2000);
    }
    //************ Successful DHCP ****************
    Serial.print("IP Address is ");
    Serial.println(sim808.getIPAddress());
    isInitTCP=1;
}

void sendSMS()
{
  Serial.println("Start to send message ...");
  
  Serial.println(MESSAGE);
  Serial.println(phone);
  
  sim808.sendSMS(phone,MESSAGE);
}
