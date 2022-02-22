#include <JY901.h>
//#include "Arduino.h"
//#include "heltec.h"
//#include "image.h"

#define BAND    868E6 

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t   mx, my, mz;

float heading;
float tiltheading;

float Axyz[3];
float Gxyz[3];
float Mxyz[3];


#define sample_num_mdate  5000

volatile float mx_sample[3];
volatile float my_sample[3];
volatile float mz_sample[3];

static float mx_centre = 0;
static float my_centre = 0;
static float mz_centre = 0;

volatile float mx_max = 0;
volatile float my_max = 0;
volatile float mz_max = 0;

volatile float mx_min = 0;
volatile float my_min = 0;
volatile float mz_min = 0;

float magCalibration[3] = {0, 0, 0}, magbias[3] = {0, 0, 0};  // Factory mag calibration and mag bias
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};      // Bias corrections for gyro and accelerometer


/*void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}*/


void setup() {
  Serial.begin(115200);
  /*Heltec.begin(true,false,true);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();*/
  JY901.startIIC();

  /*Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->fontColor(TS_8b_White,TS_8b_Black);
  Heltec.display->setFont(ArialMT_Plain_12);
  Heltec.display->setCursor(20,15); 


      //Draw an arrow pointing top
    Heltec.display.drawLine(5,20,10,10,TS_8b_White);
    Heltec.display.drawLine(10,40,10,10,TS_8b_White);      
    Heltec.display.drawLine(15,20,10,10,TS_8b_White);*/ 

  //Mxyz_init_calibrated ();

}

void loop() {
  // put your main code here, to run repeatedly:
  getAccel_Data();
  getGyro_Data();
  getCompassDate_calibrated();
  getHeading();               //before we use this function we should run 'getCompassDate_calibrated()' frist, so that we can get calibrated data ,then we can get correct angle .
  getTiltHeading();

    Serial.println("calibration parameter: ");
    Serial.print(mx_centre);
    Serial.print("         ");
    Serial.print(my_centre);
    Serial.print("         ");
    Serial.println(mz_centre);
    Serial.println("     ");


    Serial.println("Acceleration(g) of X,Y,Z:");
    Serial.print(Axyz[0]);
    Serial.print(",");
    Serial.print(Axyz[1]);
    Serial.print(",");
    Serial.println(Axyz[2]);
    Serial.println("Gyro(degress/s) of X,Y,Z:");
    Serial.print(Gxyz[0]);
    Serial.print(",");
    Serial.print(Gxyz[1]);
    Serial.print(",");
    Serial.println(Gxyz[2]);
    Serial.println("Compass Value of X,Y,Z:");
    Serial.print(Mxyz[0]);
    Serial.print(",");
    Serial.print(Mxyz[1]);
    Serial.print(",");
    Serial.println(Mxyz[2]);
    Serial.println("The clockwise angle between the magnetic north and X-Axis:");
    Serial.print(heading);
    Serial.println(" ");
    Serial.println("The clockwise angle between the magnetic north and the projection of the positive X-Axis in the horizontal plane:");
    Serial.println(tiltheading);
    Serial.println("   ");

    /*temperature = Barometer.bmp180GetTemperature(Barometer.bmp180ReadUT()); //Get the temperature, bmp180ReadUT MUST be called first
    pressure = Barometer.bmp180GetPressure(Barometer.bmp180ReadUP());//Get the temperature
    altitude = Barometer.calcAltitude(pressure); //Uncompensated caculation - in Meters
    atm = pressure / 101325;
  
    Serial.print("Temperature: ");
    Serial.print(temperature, 2); //display 2 decimal places
    Serial.println("deg C");

    Serial.print("Pressure: ");
    Serial.print(pressure, 0); //whole number only.
    Serial.println(" Pa");

    Serial.print("Ralated Atmosphere: ");
    Serial.println(atm, 4); //display 4 decimal places

    Serial.print("Altitude: ");
    Serial.print(altitude, 2); //display 2 decimal places
    Serial.println(" m");
    */
    Serial.println();
    delay(1000);

}
void Mxyz_init_calibrated ()
{

    Serial.println(F("Before using 9DOF,we need to calibrate the compass frist,It will takes about 2 minutes."));
    Serial.print("  ");
    Serial.println(F("During  calibratting ,you should rotate and turn the 9DOF all the time within 2 minutes."));
    Serial.print("  ");
    Serial.println(F("If you are ready ,please sent a command data 'ready' to start sample and calibrate."));
    while (!Serial.find("ready"));
    Serial.println("  ");
    Serial.println("ready");
    Serial.println("Sample starting......");
    Serial.println("waiting ......");

    get_calibration_Data ();

    Serial.println("     ");
    Serial.println("compass calibration parameter ");
    Serial.print(mx_centre);
    Serial.print("     ");
    Serial.print(my_centre);
    Serial.print("     ");
    Serial.println(mz_centre);
    Serial.println("    ");
}


void get_calibration_Data ()
{
    for (int i = 0; i < sample_num_mdate; i++)
    {
        get_one_sample_date_mxyz();
        


        Serial.println(i);



        if (mx_sample[2] >= mx_sample[1])mx_sample[1] = mx_sample[2];
        if (my_sample[2] >= my_sample[1])my_sample[1] = my_sample[2]; //find max value
        if (mz_sample[2] >= mz_sample[1])mz_sample[1] = mz_sample[2];

        if (mx_sample[2] <= mx_sample[0])mx_sample[0] = mx_sample[2];
        if (my_sample[2] <= my_sample[0])my_sample[0] = my_sample[2]; //find min value
        if (mz_sample[2] <= mz_sample[0])mz_sample[0] = mz_sample[2];

    }

    
    mx_max = mx_sample[1];
    my_max = my_sample[1];
    mz_max = mz_sample[1];

        Serial.print(mx_sample[1]);
        Serial.print(" ");
        Serial.print(my_sample[1]);                            //you can see the sample data here .
        Serial.print(" ");
        Serial.println(mz_sample[1]);

        Serial.print(mx_max);
        Serial.print(" ");
        Serial.print(my_max);                            //you can see the sample data here .
        Serial.print(" ");
        Serial.println(mz_max);
        


    mx_min = mx_sample[0];
    my_min = my_sample[0];
    mz_min = mz_sample[0];

        Serial.print(mx_sample[0]);
        Serial.print(" ");
        Serial.print(my_sample[0]);                            //you can see the sample data here .
        Serial.print(" ");
        Serial.println(mz_sample[0]);

        Serial.print(mx_min);
        Serial.print(" ");
        Serial.print(my_min);                            //you can see the sample data here .
        Serial.print(" ");
        Serial.println(mz_min);

    mx_centre = (mx_max + mx_min) / 2;
    my_centre = (my_max + my_min) / 2;
    mz_centre = (mz_max + mz_min) / 2;

}






void get_one_sample_date_mxyz()
{
    getCompass_Data();
    mx_sample[2] = Mxyz[0];
    my_sample[2] = Mxyz[1];
    mz_sample[2] = Mxyz[2];
}


void getCompassDate_calibrated ()
{
    getCompass_Data();
    Mxyz[0] = Mxyz[0] - mx_centre;
    Mxyz[1] = Mxyz[1] - my_centre;
    Mxyz[2] = Mxyz[2] - mz_centre;
}

void getHeading(void)
{
    heading = 180 * atan2(Mxyz[1], Mxyz[0]) / PI;
    if (heading < 0) heading += 360;
}

void getTiltHeading(void)
{
    float pitch = asin(-Axyz[0]);
    float roll = asin(Axyz[1] / cos(pitch));

    float xh = Mxyz[0] * cos(pitch) + Mxyz[2] * sin(pitch);
    float yh = Mxyz[0] * sin(roll) * sin(pitch) + Mxyz[1] * cos(roll) - Mxyz[2] * sin(roll) * cos(pitch);
    float zh = -Mxyz[0] * cos(roll) * sin(pitch) + Mxyz[1] * sin(roll) + Mxyz[2] * cos(roll) * cos(pitch);
    tiltheading = 180 * atan2(yh, xh) / PI;
    if (yh < 0)    tiltheading += 360;
}

void getAccel_Data(void)
{
    Axyz[0] = (double) JY901.getAccX() / 16384;
    Axyz[1] = (double) JY901.getAccY() / 16384;
    Axyz[2] = (double) JY901.getAccZ() / 16384;
}

void getGyro_Data(void)
{
    Gxyz[0] = (double) JY901.getGyroX() * 250 / 32768;
    Gxyz[1] = (double) JY901.getGyroY() * 250 / 32768;
    Gxyz[2] = (double) JY901.getGyroZ() * 250 / 32768;
}

void getCompass_Data(void)
{


    Mxyz[0] = (double) JY901.getMagX() * 1200 / 4096;
    Mxyz[1] = (double) JY901.getMagY() * 1200 / 4096;
    Mxyz[2] = (double) JY901.getMagZ() * 1200 / 4096;
}
