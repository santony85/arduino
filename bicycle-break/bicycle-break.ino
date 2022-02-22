#include <JY901.h>
#include <SoftwareSerial.h>
SoftwareSerial ssMPU(12, 14);



float     Gyro_x,     Gyro_y,     Gyro_z;
float     Angle_x,    Angle_y,     Angle_z;
int mag_x, mag_y, mag_z;
int     temperature;
float   Declination = +22.5833;                                             //  Degrees ... replace this declination with yours
int     Heading;

int     Mag_x,                Mag_y,                Mag_z;                  // Raw magnetometer readings
float     Roll,    Pitch,     Yaw;

// ----- Accelerometer
float    Accel_x,      Accel_y,      Accel_z,    Accel_total_vector;


unsigned char Re_buf[11],counter=0;
unsigned char sign=0;
float a[3],w[3],angle[3],T,m[3];

int mX0, mX1, mX_out;
int mY0, mY1, mY_out;
int mZ0, mZ1, mZ_out;

float heading, headingDegrees, headingFiltered, declination;

float Xm,Ym,Zm;

void read_mpu_6050_data(){
   //Serial.println(sign); 
  if(sign){ 
     Serial.println(sign);
     sign=0;
     if(Re_buf[0]==0x55){  
      Serial.println(Re_buf [1],HEX);
       switch(Re_buf [1]){
          case 0x51:
            Accel_x = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*16;             // Combine MSB,LSB Accel_x variable
            Accel_y = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*16;             // Combine MSB,LSB Accel_y variable
            Accel_z = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*16;
            temperature = (short(Re_buf [9]<<8| Re_buf [8])); 
          break;
          case 0x52:
            Gyro_x = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*2000;
            Gyro_y = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*2000;
            Gyro_z = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*2000;
            temperature = (short(Re_buf [9]<<8| Re_buf [8]));
          break;
          case 0x54:
            Mag_x = (short(Re_buf [3]<<8| Re_buf [2]));
            Mag_y = (short(Re_buf [5]<<8| Re_buf [4]));
            Mag_z = (short(Re_buf [7]<<8| Re_buf [6]));
            temperature = (short(Re_buf [9]<<8| Re_buf [8]));
            Serial.print(Mag_x);
  Serial.print(" ");
  Serial.print(Mag_y);
  Serial.print(" ");
  Serial.print(Mag_z);
  Serial.print(" ");
          break;
          case 0x53:
            Roll = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*180;
            Pitch = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*180;
            Yaw = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*180;
            float accel = sqrt(pow(Roll,2) + pow(Pitch,2) + pow(Yaw,2));
            temperature = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;
          break;
         }
         //Serial.println(Yaw);  
    }
  } 
}
    

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ssMPU.begin(9600);
  JY901.attach(ssMPU);

}

void loop() {
  //JY901.receiveSerialData(); 
  // put your main code here, to run repeatedly:
  serialEvent();
  delay(10);
  read_mpu_6050_data();
  //getCompass();

  //delay(SAMPLING_TIME_MS);
}


void getCompass(){
  //serialEvent();
  //reaad_mpu_6050_data();
  Xm = Mag_x*0.00092;
  Ym = Mag_y*0.00092;
  Zm = Mag_z*0.00092;
  heading = atan2(Ym, Xm);
  declination = 0.0043; 
  heading += declination;
  
  // Correcting when signs are reveresed
  if(heading <0) heading += 2*PI;

  // Correcting due to the addition of the declination angle
  if(heading > 2*PI)heading -= 2*PI;

  headingDegrees = heading * 180/PI; // The heading in Degrees unit

  // Smoothing the output angle / Low pass filter 
  headingFiltered = headingFiltered*0.85 + headingDegrees*0.15;

  //Sending the heading value through the Serial Port to Processing IDE
  
  Serial.println(headingDegrees);


  String cardinal;



  float headingRadians = atan2(Mag_y, Mag_x);
  headingDegrees = headingRadians * 180 / PI;
  float declinationAngle = 11.41666666666667;

  headingDegrees += declinationAngle;

  if (headingDegrees < 0) {
    headingDegrees += 360;
  }

  if (headingDegrees > 348.75 || headingDegrees < 11.25) {
    cardinal = " N";
  }
  else if (headingDegrees > 11.25 && headingDegrees < 33.75) {
    cardinal = " NNE";
  }
  else if (headingDegrees > 33.75 && headingDegrees < 56.25) {
    cardinal = " NE";
  }
  else if (headingDegrees > 56.25 && headingDegrees < 78.75) {
    cardinal = " ENE";
  }
  else if (headingDegrees > 78.75 && headingDegrees < 101.25) {
    cardinal = " E";
  }
  else if (headingDegrees > 101.25 && headingDegrees < 123.75) {
    cardinal = " ESE";
  }
  else if (headingDegrees > 123.75 && headingDegrees < 146.25) {
    cardinal = " SE";
  }
  else if (headingDegrees > 146.25 && headingDegrees < 168.75) {
    cardinal = " SSE";
  }
  else if (headingDegrees > 168.75 && headingDegrees < 191.25) {
    cardinal = " S";
  }
  else if (headingDegrees > 191.25 && headingDegrees < 213.75) {
    cardinal = " SSW";
  }
  else if (headingDegrees > 213.75 && headingDegrees < 236.25) {
    cardinal = " SW";
  }
  else if (headingDegrees > 236.25 && headingDegrees < 258.75) {
    cardinal = " WSW";
  }
  else if (headingDegrees > 258.75 && headingDegrees < 281.25) {
    cardinal = " W";
  }
  else if (headingDegrees > 281.25 && headingDegrees < 303.75) {
    cardinal = " WNW";
  }
  else if (headingDegrees > 303.75 && headingDegrees < 326.25) {
    cardinal = " NW";
  }
  else if (headingDegrees > 326.25 && headingDegrees < 348.75) {
    cardinal = " NNW";
  }

  Serial.print("Heading: ");
  Serial.print(headingDegrees);
  Serial.println(cardinal);

  delay(250);

}

void getAccell(){

  //int x =JY901.getMagX();
  //int y =JY901.getMagY();
  //int z =JY901.getMagZ();

  ///sprintf(str, "%d %d %d", x, y, z);
  Serial.print(Mag_x);
  Serial.print(" ");
  Serial.print(Mag_x);
  Serial.print(" ");
  Serial.print(Mag_z);
  //Serial.print(" ");
  //Serial.print(z);
  
  

  float accel = sqrt(pow(Mag_x,2) + pow(Mag_y,2) + pow(Mag_z,2));

  Serial.print(" ");
  Serial.println(accel);

  //if(z >= TRESHOLD_NEG && z <= TRESHOLD_POS){    }
  //Serial.println(accel);   
}

void serialEvent() {
  while (ssMPU.available()) {
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code 
  
    Re_buf[counter]=(unsigned char)ssMPU.read();
    if(counter==0&&Re_buf[0]!=0x55) return;      //第0号数据不是帧头              
    counter++;       
    if(counter==11)             //接收到11个数据
    {    
       counter=0;               //重新赋值，准备下一帧数据的接收 
       sign=1;
    }
      
  }
}
