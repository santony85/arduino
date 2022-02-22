void connectGSM (String cmd, char *res,bool debug=true) {
  ssGSM.println(cmd);
  while (1) {
    while (ssGSM.available() > 0) {
      if (ssGSM.find(res)) {
        if(debug)Serial.println("f->OK");
        return;
      }
    }
  }
}

String getMacAddress() {
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    char baseMacChr[18] = {0};
    sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    return String(baseMacChr);
}

String getValue(String data, char separator, int index){
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

void initSMS(){
 
   connectGSM("AT+CPMS=\"ME\"", "OK");
   connectGSM("AT+CMGD=1,4", "OK");
   connectGSM("AT+CMGF=1", "OK");
   connectGSM("AT+CNMI=2,2,0,0,0", "OK");
   connectGSM("AT+GPS=1", "OK");

}


void initGPRS() {
  while (1) {
    connectGSM("AT+CREG=0", "OK");
    connectGSM("AT+CREG?", "OK");
    connectGSM("AT+CIPSTATUS?", "OK");
    connectGSM("AT+CGATT=1", "OK");
    connectGSM("AT+CGDCONT=1,\"IP\",\"SL2SFR\"", "OK");
    connectGSM("AT+CGACT=1,1", "OK");
    connectGSM("AT+CIPSTART=\"TCP\",\"51.68.125.223\",3333","CONNECT OK\r\n\r\nOK");
 
    return;
  }
}

void sendMQTT(String message){
  connectGSM("AT+MQTTPUB=\"boat_"+myMacAdr+"\",\""+message+"\",0,0,0","",false); 
}

void initGPRSMQTT() {
    myMacAdr = getMacAddress(); 
    connectGSM("AT+CREG=0", "OK");
    connectGSM("AT+CREG?", "OK");
    connectGSM("AT+CIPSTATUS?", "OK");
    connectGSM("AT+CGDCONT=1,\"IP\",\"SL2SFR\"", "OK");
    connectGSM("AT+CGATT=1", "OK");
    connectGSM("AT+CGACT=1,1", "OK");
    connectGSM("AT+MQTTCONN=\"51.68.125.223\",1883,\"boat_"+myMacAdr+"\",120,0", "OK");
    connectGSM("AT+MQTTSUB=\"boat_"+myMacAdr+"\",1,0","OK");
    //connectGSM("AT+MQTTPUB=\"mqttx_"+myMacAdr+"\",\"PAYLOAD\",0,0,0",""); 
}

void initGPS(String Sec){
  connectGSM("AT+GPSRD="+Sec, "OK");
}

void waitReady(){
  digitalWrite(RST_KEY, LOW);
  delay(200);
  digitalWrite(RST_KEY, HIGH);
  delay(500);
  while (1) {
    while (ssGSM.available() > 0) {
      Serial.print('.');
      String val = ssGSM.readString();
      if (val.indexOf("READY") >= 0) {
        Serial.println();
        ssGSM.flush();
        return;
        }    
      }
    }
}

String getFromGSM (String cmd, String res, int pos, char sep) {
  ssGSM.println(cmd);
  while (1) {
    while (ssGSM.available() > 0) {
      String val= ssGSM.readString();
      Serial.println(val);
      if (val.indexOf(res) >= 0) {
        return getValue(val, sep, pos);
        } 
      } 
    }
  
}

void loopGSM(){
  if (ssGSM.available() > 0 ) {
    Serial.println(ssGSM.readString());
    }
}
