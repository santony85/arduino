#include <JY901.h>
float Accel_x, Accel_y, Accel_z;
float Gyro_x, Gyro_y, Gyro_z;
float Mag_x, Mag_y, Mag_z;
float Q0, Q1, Q2, Q3;
float D;
float Roll, Pitch, Yaw, Heading;


String makeMpuDataSms() {
  return
    "GYRO / COMPASS\r\n  Roll: " + String(Roll,3) + "\r\n" +
    "  Pitch: " + String(Pitch,3) + "\r\n" +
    "  Yaw: "  + String(Yaw,3) + "\r\n" +
    "  Heading: "  + String(Heading,3) + "\r\n" +
    "\r\n" +
    "  Acc X: " + String(Accel_x,4) + "\r\n" +
    "  Acc Y: " + String(Accel_y,4) + "\r\n" +
    "  Acc Z: " + String(Accel_z,4) + "\r\n" +
    "\r\n" +
    "  Gyro X: " + String(Gyro_x,4) + "\r\n" +
    "  Gyro Y: " + String(Gyro_y,4) + "\r\n" +
    "  Gyro Z: " + String(Gyro_z,4) + "\r\n" +
    "\r\n" +
    "  Mag X: " + String(Mag_x,4) + "\r\n" +
    "  Mag Y: " + String(Mag_y,4) + "\r\n" +
    "  Mag Z: " + String(Mag_z,4) + "\r\n";
}

String makeMpuDataJson() {
  //navs,nbsat,hdop,vfix
  return
    "{'table':{'name':'compass','Roll':'" + String(Roll,3) + "'," +
    "'Pitch': '" + String(Pitch,3) + "'," +
    "'Yaw': '"  + String(Yaw,3) + "'," +
    "'Heading': '"  + String(Heading,3) + "'," +
    "'Acc_X': '" + String(Accel_x,4) + "'," +
    "'Acc_Y': '" + String(Accel_y,4) + "'," +
    "'Acc_Z': '" + String(Accel_z,4) + "'," +
    "'Gyro_X': '" + String(Gyro_x,4) + "'," +
    "'Gyro_Y': '" + String(Gyro_y,4) + "'," +
    "'Gyro_Z': '" + String(Gyro_z,4) + "'," +
    "'Mag_X': '" + String(Mag_x,4) + "'," +
    "'Mag_Y': '" + String(Mag_y,4) + "'," +
    "'Mag_Z': '" + String(Mag_z,4) + "'}},";   
}

void makeMPUData(){
  Accel_x = JY901.getAccX();
  Accel_y = JY901.getAccY();
  Accel_z = JY901.getAccZ();
  Gyro_x = JY901.getGyroX();
  Gyro_y = JY901.getGyroY();
  Gyro_z = JY901.getGyroZ();
  Mag_x = JY901.getMagX();
  Mag_y = JY901.getMagY();
  Mag_z = JY901.getMagZ();
  Roll = JY901.getRoll();
  Pitch = JY901.getPitch();
  Yaw = JY901.getYaw();
  Heading = 180 * atan2(Mag_y,Mag_x)/PI;
  if(Heading < 0)Heading += 360;
  //Serial.println(Heading);
  //TelnetStream.println(Heading);

  }
