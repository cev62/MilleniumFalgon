#ifndef GYRO_H
#define GYRO_H

#include <Wire.h>
#include "Arduino.h"

#define MPU6050_WHO_AM_I           0x75   // R
#define MPU6050_PWR_MGMT_1         0x6B   // R/W
#define MPU6050_ACCEL_XOUT_H       0x3B   // R  
#define MPU6050_I2C_ADDRESS 0x68

class GyroAccel
{
public:
  GyroAccel();
  void Reset();
  void Update();
  float *angular_velocity, *angle, *acceleration, *velocity, *pos;
private:
  void MPU6050_read(int start, uint8_t *buffer, int size);
  void MPU6050_write(int start, const uint8_t *pData, int size);
  void MPU6050_write_reg(int reg, uint8_t data);
  void GetAngleRateInDegrees(int64_t *angles, float *degrees);
  void GetAccelInMeters(int64_t *accels, float *meters);

  typedef union accel_t_gyro_union
  {
    struct
    {
      uint8_t x_accel_h;
      uint8_t x_accel_l;
      uint8_t y_accel_h;
      uint8_t y_accel_l;
      uint8_t z_accel_h;
      uint8_t z_accel_l;
      uint8_t t_h;
      uint8_t t_l;
      uint8_t x_gyro_h;
      uint8_t x_gyro_l;
      uint8_t y_gyro_h;
      uint8_t y_gyro_l;
      uint8_t z_gyro_h;
      uint8_t z_gyro_l;
    } reg;
    struct
    {
      int16_t x_accel;
      int16_t y_accel;
      int16_t z_accel;
      int16_t temperature;
      int16_t x_gyro;
      int16_t y_gyro;
      int16_t z_gyro;
    } value;
  };
  
  int64_t *raw_angular_velocity, *raw_angle, *raw_acceleration, *raw_velocity, *raw_pos;
  
  long int timer;
  int dt;
  accel_t_gyro_union accel_t_gyro;
  
};

#endif
