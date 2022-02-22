#include <SoftwareSerial.h>
SoftwareSerial ssMPU(D5,D4);
unsigned char Re_buf[11],counter=0;
unsigned char sign=0;
float a[3],w[3],angle[3],T,m[3];

#define Frequency 125                                                   // 8mS sample interval 
#define Sensitivity 65.5  

#define Loop_time 1000000/Frequency                                     // Loop time (uS)
long    Loop_start;    // Gyro sensitivity (see data sheet)

long Loop_start_time;
long Debug_start_time;

#define Sensor_to_deg 1/(Sensitivity*Frequency)                         // Convert sensor reading to degrees
#define Sensor_to_rad Sensor_to_deg*DEG_TO_RAD    

int     Gyro_x,     Gyro_y,     Gyro_z;
long    Gyro_x_cal, Gyro_y_cal, Gyro_z_cal;
float   Gyro_pitch, Gyro_roll, Gyro_yaw;
float   Gyro_pitch_output, Gyro_roll_output;
int mag_x, mag_y, mag_z;
int     temperature;
int status_reg_2;

float   Declination = +22.5833;                                             //  Degrees ... replace this declination with yours
int     Heading;

int     Mag_x,                Mag_y,                Mag_z;                  // Raw magnetometer readings
float   Mag_x_dampened,       Mag_y_dampened,       Mag_z_dampened;
float   Mag_x_hor, Mag_y_hor;
float   Mag_pitch, Mag_roll;

// ----- Record compass offsets, scale factors, & ASA values
/*
   These values seldom change ... an occasional check is sufficient
   (1) Open your Arduino "Serial Monitor
   (2) Set "Record_data=true;" then upload & run program.
   (3) Replace the values below with the values that appear on the Serial Monitor.
   (4) Set "Record_data = false;" then upload & rerun program.
*/
bool    Record_data = false;
int     Mag_x_offset = 46,      Mag_y_offset = 190,     Mag_z_offset = -254;   // Hard-iron offsets
float   Mag_x_scale = 1.01,     Mag_y_scale = 0.99,     Mag_z_scale = 1.00;    // Soft-iron scale factors
float   ASAX = 1.17,            ASAY = 1.18,            ASAZ = 1.14;           // (A)sahi (S)ensitivity (A)djustment fuse ROM values.


// ----- Accelerometer
long    Accel_x,      Accel_y,      Accel_z,    Accel_total_vector;
float   Accel_pitch,  Accel_roll;



bool Gyro_synchronised = false;
bool Flag = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");
  ssMPU.begin(9600);
  delay(2000);
  configure_magnetometer();
  calibrate_magnetometer();
  calibrate_gyro();
}

void read_mpu_6050_data(){
  
  if(sign){  
     sign=0;
     if(Re_buf[0]==0x55){  
       switch(Re_buf [1]){
          case 0x51:
          Accel_x = (short(Re_buf [3]<<8| Re_buf [2]));             // Combine MSB,LSB Accel_x variable
          Accel_y = (short(Re_buf [5]<<8| Re_buf [4]));             // Combine MSB,LSB Accel_y variable
          Accel_z = (short(Re_buf [7]<<8| Re_buf [6]));
          temperature = (short(Re_buf [9]<<8| Re_buf [8])); 
       break;
       case 0x52:
         Gyro_x = (short(Re_buf [3]<<8| Re_buf [2]));
         Gyro_y = (short(Re_buf [5]<<8| Re_buf [4]));
         Gyro_z = (short(Re_buf [7]<<8| Re_buf [6]));
         temperature = (short(Re_buf [9]<<8| Re_buf [8]));
       break;
       /*case 0x54:
          m[0] = (short(Re_buf [3]<<8| Re_buf [2]));
          m[1] = (short(Re_buf [5]<<8| Re_buf [4]));
          m[2] = (short(Re_buf [7]<<8| Re_buf [6]));
          temperature = (short(Re_buf [9]<<8| Re_buf [8]));
       break;*/
       /*case 0x53:
       angle[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*180;
       angle[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*180;
       angle[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*180;
       float accel = sqrt(pow(a[0],2) + pow(a[1],2) + pow(a[2],2));
       temperature = (short(Re_buf [9]<<8| Re_buf [8]));
   
                //Serial.print("a:");
                Serial.print(a[0]);Serial.print(" ");
                Serial.print(a[1]);Serial.print(" ");
                Serial.print(a[2]);Serial.print(" ");
                /*Serial.print("w:");
                Serial.print(w[0]);Serial.print(" ");
                Serial.print(w[1]);Serial.print(" ");
                Serial.print(w[2]);Serial.print(" ");
                /*Serial.print("angle:");
                Serial.print(angle[0]);Serial.print(" ");
                Serial.print(angle[1]);Serial.print(" ");
                Serial.print(angle[2]);Serial.print(" ");
                
                //Serial.print("mag:");
                Serial.print(m[0]);Serial.print(" ");
                Serial.print(m[1]);Serial.print(" ");
                Serial.print(m[2]);Serial.print(" ");
                //Serial.print("acc:");
                Serial.println(accel);
                /*Serial.print(" ");
                Serial.print("T:");
                Serial.println(T);
                break;*/
          } 
    }
  } 
  }

void configure_magnetometer(){
  serialEvent();
  if (sign){
      sign=0;
      if(Re_buf[0]==0x55){  
        switch(Re_buf [1]){
          case 0x54:
            ASAX = ((short(Re_buf [3]<<8| Re_buf [2])) - 128) * 0.5 / 128 + 1; 
            ASAY = ((short(Re_buf [5]<<8| Re_buf [2])) - 128) * 0.5 / 128 + 1;
            ASAZ = ((short(Re_buf [7]<<8| Re_buf [2])) - 128) * 0.5 / 128 + 1;
            delay(100);
            break;
        }
      }      
  }
}

void read_magnetometer(){
      if (sign){
      sign=0;
      if(Re_buf[0]==0x55){  
        switch(Re_buf [1]){
          case 0x54:
            mag_x = (short(Re_buf [3]<<8| Re_buf [2])) * ASAX;            // Combine LSB,MSB X-axis, apply ASA corrections
            mag_y = (short(Re_buf [5]<<8| Re_buf [2])) * ASAY;            // Combine LSB,MSB Y-axis, apply ASA corrections
            mag_z = (short(Re_buf [7]<<8| Re_buf [2])) * ASAZ; 
            Mag_x = (mag_x - Mag_x_offset) * Mag_x_scale;
            Mag_y = (mag_y - Mag_y_offset) * Mag_y_scale;
            Mag_z = (mag_z - Mag_z_offset) * Mag_z_scale;
            break;
            }
          }
            
      }
    
}

void calibrate_magnetometer(){
  Record_data=true;
  // ----- Locals
  int mag_x, mag_y, mag_z;
  int status_reg_2;                                               // ST2 status register

  int mag_x_min =  32767;                                         // Raw data extremes
  int mag_y_min =  32767;
  int mag_z_min =  32767;
  int mag_x_max = -32768;
  int mag_y_max = -32768;
  int mag_z_max = -32768;

  float chord_x,  chord_y,  chord_z;                              // Used for calculating scale factors
  float chord_average;

  // Set the LCD cursor to column 0 line 
  // ----- Record min/max XYZ compass readings
  for (int counter = 0; counter < 16000 ; counter ++){
    serialEvent();
    Loop_start = micros();                                        // Start loop timer
    if (counter % 1000 == 0){
      Serial.println(counter);                        // Print a dot on the LCD every 1000 readings                                // Wait for the data
      }
    if (sign){
      sign=0;
      if(Re_buf[0]==0x55){  
        switch(Re_buf [1]){
          case 0x54:
            mag_x = (short(Re_buf [3]<<8| Re_buf [2])) * ASAX;            // Combine LSB,MSB X-axis, apply ASA corrections
            mag_y = (short(Re_buf [5]<<8| Re_buf [2])) * ASAY;            // Combine LSB,MSB Y-axis, apply ASA corrections
            mag_z = (short(Re_buf [7]<<8| Re_buf [2])) * ASAZ; 
            mag_x_min = min(mag_x, mag_x_min);
            mag_x_max = max(mag_x, mag_x_max);
            mag_y_min = min(mag_y, mag_y_min);
            mag_y_max = max(mag_y, mag_y_max);
            mag_z_min = min(mag_z, mag_z_min);
            mag_z_max = max(mag_z, mag_z_max);
            break;
            }
          }
       
      }
    ssMPU.flush();
    delay(4);                                                     // Time interval between magnetometer readings
  }

  // ----- Calculate hard-iron offsets
  Mag_x_offset = (mag_x_max + mag_x_min) / 2;                     // Get average magnetic bias in counts
  Mag_y_offset = (mag_y_max + mag_y_min) / 2;
  Mag_z_offset = (mag_z_max + mag_z_min) / 2;

  // ----- Calculate soft-iron scale factors
  chord_x = ((float)(mag_x_max - mag_x_min)) / 2;                 // Get average max chord length in counts
  chord_y = ((float)(mag_y_max - mag_y_min)) / 2;
  chord_z = ((float)(mag_z_max - mag_z_min)) / 2;

  chord_average = (chord_x + chord_y + chord_z) / 3;              // Calculate average chord length

  Mag_x_scale = chord_average / chord_x;                          // Calculate X scale factor
  Mag_y_scale = chord_average / chord_y;                          // Calculate Y scale factor
  Mag_z_scale = chord_average / chord_z;                          // Calculate Z scale factor

  // ----- Record magnetometer offsets
  /*
     When active this feature sends the magnetometer data
     to the Serial Monitor then halts the program.
  */ 
  if (Record_data == true)
  {
    // ----- Display data extremes
    Serial.print("XYZ Max/Min: ");
    Serial.print(mag_x_min); Serial.print("\t");
    Serial.print(mag_x_max); Serial.print("\t");
    Serial.print(mag_y_min); Serial.print("\t");
    Serial.print(mag_y_max); Serial.print("\t");
    Serial.print(mag_z_min); Serial.print("\t");
    Serial.println(mag_z_max);
    Serial.println("");

    // ----- Display hard-iron offsets
    Serial.print("Hard-iron: ");
    Serial.print(Mag_x_offset); Serial.print("\t");
    Serial.print(Mag_y_offset); Serial.print("\t");
    Serial.println(Mag_z_offset);
    Serial.println("");

    // ----- Display soft-iron scale factors
    Serial.print("Soft-iron: ");
    Serial.print(Mag_x_scale); Serial.print("\t");
    Serial.print(Mag_y_scale); Serial.print("\t");
    Serial.println(Mag_z_scale);
    Serial.println("");

    // ----- Display fuse ROM values
    Serial.print("ASA: ");
    Serial.print(ASAX); Serial.print("\t");
    Serial.print(ASAY); Serial.print("\t");
    Serial.println(ASAZ);

    // ----- Halt program
    //while (true);                                       // Wheelspin ... program halt
  }
}

void serialEvent() {
  while (ssMPU.available()) {
    
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code 
  
    Re_buf[counter]=(unsigned char)ssMPU.read();
    if(counter==0&&Re_buf[0]!=0x55) return;
    counter++;       
    if(counter==11){    
       counter=0; 
       sign=1;
    }
      
  }
}

void calibrate_gyro(){
    for (int counter = 0; counter < 2000 ; counter ++)    //Run this code 2000 times
  {
    serialEvent();
    Loop_start = micros();
    if (counter % 125 == 0)Serial.print(".");              //Print a dot on the LCD every 125 readings
    read_mpu_6050_data();                               //Read the raw acc and gyro data from the MPU-6050
    Gyro_x_cal += Gyro_x;                               //Add the gyro x-axis offset to the gyro_x_cal variable
    Gyro_y_cal += Gyro_y;                               //Add the gyro y-axis offset to the gyro_y_cal variable
    Gyro_z_cal += Gyro_z;                               //Add the gyro z-axis offset to the gyro_z_cal variable
    delay(5);           // Wait until "Loop_time" microseconds have elapsed
  }
  Gyro_x_cal /= 2000;                                   //Divide the gyro_x_cal variable by 2000 to get the average offset
  Gyro_y_cal /= 2000;                                   //Divide the gyro_y_cal variable by 2000 to get the average offset
  Gyro_z_cal /= 2000;  
  }

void loop() {
  read_mpu_6050_data();                                             // Read the raw acc and gyro data from the MPU-6050

  // ----- Adjust for offsets
  Gyro_x -= Gyro_x_cal;                                             // Subtract the offset from the raw gyro_x value
  Gyro_y -= Gyro_y_cal;                                             // Subtract the offset from the raw gyro_y value
  Gyro_z -= Gyro_z_cal;                                             // Subtract the offset from the raw gyro_z value

  // ----- Calculate travelled angles
  /*
    ---------------------------
    Adjust Gyro_xyz signs for:
    ---------------------------
    Pitch (Nose - up) = +ve reading
    Roll (Right - wing down) = +ve reading
    Yaw (Clock - wise rotation)  = +ve reading
  */
  Gyro_pitch += -Gyro_y * Sensor_to_deg;                            // Integrate the raw Gyro_y readings
  Gyro_roll += Gyro_x * Sensor_to_deg;                              // Integrate the raw Gyro_x readings
  Gyro_yaw += -Gyro_z * Sensor_to_deg;                              // Integrate the raw Gyro_x readings

  // ----- Compensate pitch and roll for gyro yaw
  Gyro_pitch += Gyro_roll * sin(Gyro_z * Sensor_to_rad);            // Transfer the roll angle to the pitch angle if the Z-axis has yawed
  Gyro_roll -= Gyro_pitch * sin(Gyro_z * Sensor_to_rad);            // Transfer the pitch angle to the roll angle if the Z-axis has yawed

  // ----- Accelerometer angle calculations
  Accel_total_vector = sqrt((Accel_x * Accel_x) + (Accel_y * Accel_y) + (Accel_z * Accel_z));   // Calculate the total (3D) vector
  Accel_pitch = asin((float)Accel_x / Accel_total_vector) * RAD_TO_DEG;                         //Calculate the pitch angle
  Accel_roll = asin((float)Accel_y / Accel_total_vector) * RAD_TO_DEG;                         //Calculate the roll angle

  // ----- Zero any residual accelerometer readings
  /*
     Place the accelerometer on a level surface
     Adjust the following two values until the pitch and roll readings are zero
  */
  Accel_pitch -= -0.2f;                                             //Accelerometer calibration value for pitch
  Accel_roll -= 1.1f;                                               //Accelerometer calibration value for roll

  // ----- Correct for any gyro drift
  if (Gyro_synchronised)
  {
    // ----- Gyro & accel have been synchronised
    Gyro_pitch = Gyro_pitch * 0.9996 + Accel_pitch * 0.0004;        //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    Gyro_roll = Gyro_roll * 0.9996 + Accel_roll * 0.0004;           //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else
  {
    // ----- Synchronise gyro & accel
    Gyro_pitch = Accel_pitch;                                       //Set the gyro pitch angle equal to the accelerometer pitch angle
    Gyro_roll = Accel_roll;                                         //Set the gyro roll angle equal to the accelerometer roll angle
    Gyro_synchronised = true;                                             //Set the IMU started flag
  }

  // ----- Dampen the pitch and roll angles
  Gyro_pitch_output = Gyro_pitch_output * 0.9 + Gyro_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  Gyro_roll_output = Gyro_roll_output * 0.9 + Gyro_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value

    read_magnetometer();

  // ----- Fix the pitch, roll, & signs
  /*
     MPU-9250 gyro and AK8963 magnetometer XY axes are orientated 90 degrees to each other
     which means that Mag_pitch equates to the Gyro_roll and Mag_roll equates to the Gryro_pitch

     The MPU-9520 and AK8963 Z axes point in opposite directions
     which means that the sign for Mag_pitch must be negative to compensate.
  */
  Mag_pitch = -Gyro_roll_output * DEG_TO_RAD;
  Mag_roll = Gyro_pitch_output * DEG_TO_RAD;

  // ----- Apply the standard tilt formulas
  Mag_x_hor = Mag_x * cos(Mag_pitch) + Mag_y * sin(Mag_roll) * sin(Mag_pitch) - Mag_z * cos(Mag_roll) * sin(Mag_pitch);
  Mag_y_hor = Mag_y * cos(Mag_roll) + Mag_z * sin(Mag_roll);

  // ----- Disable tilt stabization if switch closed
//  if (!(digitalRead(Switch)))
  {
    // ---- Test equations
    Mag_x_hor = Mag_x;
    Mag_y_hor = Mag_y;
  }

  // ----- Dampen any data fluctuations
  Mag_x_dampened = Mag_x_dampened * 0.9 + Mag_x_hor * 0.1;
  Mag_y_dampened = Mag_y_dampened * 0.9 + Mag_y_hor * 0.1;

  // ----- Calculate the heading
  Heading = atan2(Mag_x_dampened, Mag_y_dampened) * RAD_TO_DEG;  // Magnetic North

  /*
     By convention, declination is positive when magnetic north
     is east of true north, and negative when it is to the west.
  */

  Heading += Declination;               // Geographic North
  if (Heading > 360.0) Heading -= 360.0;
  if (Heading < 0.0) Heading += 360.0;

  // ----- Allow for under/overflow
  if (Heading < 0) Heading += 360;
  if (Heading >= 360) Heading -= 360;

  Serial.println(Heading);
  while ((micros() - Loop_start_time) < 8000);
  Loop_start_time = micros();

}
