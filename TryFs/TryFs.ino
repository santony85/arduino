#include "LittleFS.h"

float vmin = 20;
float vmax = 26;
int   vnum = 9;
String vname = "Change Cuve";
String myIp="192.168.1.XX";
int isOnOff = 1;


void setup() {
  Serial.begin(115200);
  Serial.println("Started");
  delay(500);
  if(!LittleFS.begin()){
     Serial.println("An Error has occurred while mounting LittleFS");
     delay(1000);
     return;
  }
  File file = LittleFS.open("/cuve.txt", "r");
  if(!file){
    Serial.println("No Saved Data!");
    writeData(makeString());
   }
  
  readData();
}

void loop() {
  // put your main code here, to run repeatedly:

}


String makeString(){
    String valt="";
    valt += vnum;
    valt +=";";
    valt += vmin;
    valt +=";";
    valt += vmax;
    valt +=";";
    valt += vname;
    valt +=";";
    valt += isOnOff;
    valt +=";";
    valt += myIp;
    Serial.println(valt);
    return valt;
  }

void readData(){
  File file = LittleFS.open("/cuve.txt", "r");
   String val="";
   while(file.available()){
      char xx = (char)file.read();
      val.concat(xx);
   }
   file.close();
   //Serial.println(val);
   //split string;
    char buf[1024];
    val.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    int a=0;
    while ((str = strtok_r(p, ";", &p)) != NULL){ // delimiter is the semicolon
      if(a==0)vnum=atoi(str);
      if(a==1)vmin=atoi(str);
      if(a==2)vmax=atoi(str);
      if(a==3)vname=str;
      if(a==4)isOnOff=atoi(str);
      if(a==5)myIp=str;
      //Serial.print(a++);
      //Serial.print("->");
      //Serial.println(str);
      a++;
    }
   Serial.println(vname);
   Serial.println(vmax);
   Serial.println(vmin);
   Serial.println(vnum);
   Serial.println(isOnOff);
   Serial.println(myIp);
  }
  
void writeData(String data){
  File file = LittleFS.open("/cuve.txt", "w");
  file.print(data);
  delay(1);
  file.close();
  }
