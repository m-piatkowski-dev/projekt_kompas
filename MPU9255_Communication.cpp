/*
MPU9255_Communication.cpp - MPU9255 <-> CPU communication functions
*/

#include "MPU9255.h"
#include "Arduino.h"

//Request data
//Parameters:
// * uint8_t address    - address of the device
// * uint8_t subAddress - address of the register (the read starts from that register)
// * uint8_t bytes      - number of requested bytes
// * uint8_t *data      - pointer to output table
void MPU9255::requestBytes(uint8_t address, uint8_t subAddress, uint8_t bytes, uint8_t *data)
{
  Wire.beginTransmission(address);
  Wire.write(subAddress);
  Wire.endTransmission(false);
  Wire.requestFrom(address, bytes);
  if(Wire.available() >=1)
  {
    for(char i =0; i<bytes; i++)
    {
      data[i] = Wire.read();
    }
  }
  Wire.endTransmission(true);
}

//Read one byte of data
//Parameters:
// * uint8_t address    - device address
// * uint8_t subAddress - register address
//Returns : Readed byte of data (uint8_t)
uint8_t MPU9255::read(uint8_t address, uint8_t subAddress)
{
  Wire.beginTransmission(address);
  Wire.write(subAddress);
  Wire.endTransmission(false);
  Wire.requestFrom(address,(uint8_t) 1);//request one byte from the register
  byte data = Wire.read();//read one byte of data
  Wire.endTransmission(true);
  return data;
}

// Write one byte of data
//Parameters:
// * uint8_t address    - device address
// * uint8_t subAddress - register address
// * uint8_t data       - one byte of data that we want to put in the register
void MPU9255::write(uint8_t address, uint8_t subAddress, uint8_t data)
{
  Wire.beginTransmission(address);
  Wire.write(subAddress);
  Wire.write(data);
  Wire.endTransmission(true);
}

//Change state of the register using OR operation
//Parameters:
// * uint8_t address    - device address
// * uint8_t subAddress - register address
// * uint8_t data       - one byte of data that we want to put in the register
void MPU9255::write_OR(uint8_t address, uint8_t subAddress, uint8_t data)
{
  uint8_t c = read(address,subAddress);
  c = c | data;
  write(address,subAddress,c);
}

//Change state of the register using AND operation
//Parameters:
// * uint8_t address    - device address
// * uint8_t subAddress - register address
// * uint8_t data       - one byte of data that we want to put in the register
void MPU9255::write_AND(uint8_t address, uint8_t subAddress, uint8_t data)
{
  uint8_t c = read(address,subAddress);
  c = c & data;
  write(address,subAddress,c);
}

//read an array of bytes from the device
//Parameters:
// * uint8_t *output - pointer to the output array
// * char size       - number of bytes to read
void MPU9255::readArray(uint8_t *output, char size,uint8_t data_in)
{
  for(char i = 0; i<size; i++)
  {
    output[i] = data_in;//read byte and put it into rawData table
  }
}