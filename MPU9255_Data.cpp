/*
MPU9255_Data.cpp - Measurements functions
*/
#include "MPU9255.h"
#include "Arduino.h"
/*
//read accelerometer data registers
void MPU9255::read_acc()
{
  uint8_t rawData[6];
  requestBytes(MPU_address, ACCEL_XOUT_H, 7, rawData);//request data from the accelerometer
  //read data
  //Wire.write(rawData,6);
  //readArray(rawData,6);
  //store data in raw data variables
  ax = ((int16_t)rawData[1] << 8) | rawData[0];
  ay = ((int16_t)rawData[3] << 8) | rawData[2];
  az = ((int16_t)rawData[5] << 8) | rawData[4];
}
//read gyroscope data registers
void MPU9255::read_gyro()
{
  uint8_t rawData[6];
  requestBytes(MPU_address, GYRO_XOUT_H, 7, rawData);

  gx = ((int16_t)rawData[1] << 8) | rawData[0];
  gy = ((int16_t)rawData[3] << 8) | rawData[2];
  gz = ((int16_t)rawData[5] << 8) | rawData[4];
}*/
uint8_t MPU9255::read_mag()
{  
  uint8_t rawData[8];
  requestBytes(MAG_address, 0x02, 8, rawData);

  dor_status = rawData[0];
  if(rawData[0]==1 || rawData[0]==3)
  {
    hofl_status = rawData[7];
    if(rawData[7]!=0x08)
    {
      mx = (((int16_t)rawData[2]) << 8) | rawData[1] ;
      my = (((int16_t)rawData[4]) << 8) | rawData[3] ;
      mz = (((int16_t)rawData[6]) << 8) | rawData[5] ;
      byte checkCNTL = read(MAG_address, CNTL);//check resolution of magnetometer
      if(bitRead(checkCNTL, 4)==LOW)
      {//NORMALIZED DATA
        mxs = mx * off_asax*0.6f;//raw values of measurements multiplied by offset(off_assa*) and magnetic unit 0.6/LSB/14bit
        mys = my * off_asay*0.6f;
        mzs = mz * off_asaz*0.6f;
      }
      else
      {
        mxs = mx * off_asax*0.15f;//raw values of measurements multiplied by offset(off_assa*) and magnetic unit 0.15/LSB/16bit
        mys = my * off_asay*0.15f;
        mzs = mz * off_asaz*0.15f;
      }
      return 0;
    }return 1;
  }return 2;  
}

//Wyznaczanie całkowitego wektora magnetycznego
float MPU9255::total_mag_vector(float x, float y, float z)
{
  return sqrtf(x*x+y*y+z*z);
}
//wyznaczanie kierunku starym sposobem
float MPU9255::direction(float x, float y)
{
  float direction;
  if(x=0 && y<0)
  {
    direction=90;
  }
  else if (x=0 && y>0)
  {
    direction=270;
  }
  else if (x<0)
  {
    direction = 180 - ((atan(y/x))*RAD_TO_DEG);
  }else if (x>0 && y<0)
  {
    direction = -(atan(y/x))*RAD_TO_DEG;
  }else if (x>0 && y>0)
  {
    direction = 360 - ((atan(y/x))*RAD_TO_DEG);
  }

return direction;  
}
//SELF TEST MODE!!!
int8_t MPU9255::self_test()
{ 
  write(MAG_address, CNTL, 0x10);//set magnetometer to powerdown mode
  delay(100);
  write(MAG_address, 0x0C, 0x40);//set ASTC to SELF TEST
  delay(100);
  write(MAG_address, CNTL, 0x18);//set magnetometer to SELF TEST mode
  delay(100);
  
    byte drdy_dor = read(MAG_address, 0x02);
    byte xl = read(MAG_address, 0x03); //reading values from mag registers
    byte xh = read(MAG_address, 0x04);
    byte yl = read(MAG_address, 0x05);
    byte yh = read(MAG_address, 0x06);
    byte zl = read(MAG_address, 0x07);
    byte zh = read(MAG_address, 0x08);
    byte hofl = read(MAG_address, 0x09);

    write(MAG_address, 0x0C, 0x00);//set ASTC to be off
    delay(100);
    write(MAG_address, CNTL, 0x10);//set magnetometer to POWER DOWN mode
    delay(100);

  dor_status = drdy_dor;
  hofl_status = hofl;
  if(drdy_dor==1)
  {
    if(hofl!=1)
    {
      mx = (((int16_t)xh) << 8) | xl ;//teraz działa poprawnie -> do sprawdzenia ustawienie czułości i zaimplementowania
      my = (((int16_t)yh) << 8) | yl ;
      mz = (((int16_t)zh) << 8) | zl ;
      byte checkCNTL = read(MAG_address, CNTL);
      if(bitRead(checkCNTL, 4)==LOW)
      {
        mxs = mx * off_asax*0.6f;//raw values of measurements multiplied by offset(off_assa*) and magnetic unit 0.15/LSB/16bit or 0.6/LSB/14bit
        mys = my * off_asay*0.6f;
        mzs = mz * off_asaz*0.6f;
        if((mxs<=-50 && mxs>=50)&&(mys<=-50 && mys<=50)&&(mzs<=-800 && mzs>=-200))
        return 66;
      }
      else
      {
        mxs = mx * off_asax*0.15f;//raw values of measurements multiplied by offset(off_assa*) and magnetic unit 0.15/LSB/16bit or 0.6/LSB/14bit
        mys = my * off_asay*0.15f;
        mzs = mz * off_asaz*0.15f;
        if((mxs<=-200&&mxs>=200)&&(mys<=-200&&mys<=200)&&(mzs<=-3200&&mzs>=-800))
        return 65;
      }
      return 0;
    }return 1;
  }return 2;
}

//SESITIVITY ADJUSTMENTS !!!

void MPU9255::set_sensitivity()
{
  write(MAG_address, CNTL, 0x1F);//set reg to read ROM
  asax = read(MAG_address, 0x10);//reading ROM sensitivity values
  asay = read(MAG_address, 0x11);
  asaz = read(MAG_address, 0x12);
  write(MAG_address, CNTL, 0x10);//set reg to power down mode
  delay(100);
  write(MAG_address, CNTL, 0x12);//set reg to nromal mode 16bit and 8Hz readings

  /*
  mpu.set_sensitivity();//!!!****READ AND COUNT of sesitivity and offset of magnetometer!!!!
  tft.setCursor(2,8);//wyswietlanie rejetrow rom z danymi do ustawiania czułosci czujnika
  tft.print("ASAX:");
  tft.print(mpu.asax);
  tft.print(" | ");
  tft.println((float)(((mpu.asax-128)*0.5)/128 +1) );
  tft.print("ASAY:");
  tft.print(mpu.asay);
  tft.print(" | ");
  tft.println((float)(((mpu.asay-128)*0.5)/128 +1));
  tft.print("ASAZ:");
  tft.print(mpu.asaz);
  tft.print(" | ");
  tft.println((float)(((mpu.asaz-128)*0.5)/128 +1));
  */
}
