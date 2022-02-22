int axey = A0;   // Entrée de référence pour l'axe Y.
int axex = A3;   // Entrée de référence pour l'axe X.
int valeurX;
int valeurY;
int xVal, yVal;
int isread=0;
int cnt=0;
void setup() {  
  Serial.begin(115200);
  /* Définition des leds
  comme sorties */
  pinMode(axey, INPUT_PULLDOWN);
	pinMode(axey, INPUT_PULLDOWN);


}

void getJoyVal(){
  valeurX = analogRead(axex);
  delay(200);
  valeurY = analogRead(axey);
  xVal = map(valeurX,0,4095,0,5);
  yVal = map(valeurY,0,4095,0,5);
}
 
void loop() {  

if(isread==0){
  getJoyVal();
  if(xVal == 5 && isread==0){
    Serial.println("Move Right");
    isread=1;
    }    
  if(xVal == 0 && isread==0){
    Serial.println("Move Left");
    isread=1;
    }

  if(yVal == 0 && isread==0){
    Serial.println("Move Up");
    isread=1;
    }    
  if(yVal == 5 && isread==0){
    isread=1;
    Serial.println("Move Down");
    }    
  //delay(500);
}
else {
   getJoyVal();
   //Serial.println(xVal);
   Serial.println(yVal);
   if(xVal==4&&yVal==4){
     cnt++;
   }
   if(cnt==3){
     cnt=0;
     isread=0;
     Serial.println("timeout");     
   }
   delay(100);

}



  /*Serial.println(valeurX);
  //delay(200);
  if(valeurX<50){
    Serial.println("right");
  }
  else if(valeurX>4000){
    Serial.println("left");
  }

  delay(200);
  valeurY = analogRead(axey);
  Serial.println(valeurY);
  //delay(200);
  if(valeurY<50){
    Serial.println("light");
  }
  else if(valeurY>4000){
    Serial.println("map");
  }
  delay(200);*/
  
}