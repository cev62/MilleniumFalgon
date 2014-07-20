#include "gyro.h"

void GyroAccel::MPU6050_read(int start, uint8_t *buffer, int size)
{
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.write(start); 
  Wire.endTransmission(false);    // hold the I2C-bus
  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  int i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
}
 
void GyroAccel::MPU6050_write(int start, const uint8_t *pData, int size)
{ 
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  Wire.write(start);        // write the start address
  Wire.write(pData, size);  // write data bytes
  Wire.endTransmission(true); // release the I2C-bus
}
 
void GyroAccel::MPU6050_write_reg(int reg, uint8_t data)
{
  MPU6050_write(reg, &data, 1);
}

GyroAccel::GyroAccel()
{  
  raw_angular_velocity = new int64_t[3];
  raw_angle = new int64_t[3];
  raw_acceleration = new int64_t[3];
  raw_angle = new int64_t[3];
  raw_pos = new int64_t[3];
  
  angular_velocity = new float[3];
  angle = new float[3];
  acceleration = new float[3];
  velocity = new float[3];
  pos = new float[3];
  
  timer = millis();
 
  // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin(); 
 
  // Clear the 'sleep' bit to start the sensor.
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
}

void GyroAccel::Reset()
{
  for(int i = 0; i < 3; i++){ raw_angular_velocity[i] = 0; }
  for(int i = 0; i < 3; i++){ raw_angle[i] = 0; }
  for(int i = 0; i < 3; i++){ raw_acceleration[i] = 0; }
  for(int i = 0; i < 3; i++){ raw_velocity[i] = 0; }
  for(int i = 0; i < 3; i++){ raw_pos[i] = 0; }
  for(int i = 0; i < 3; i++){ angular_velocity[i] = 0; }
  for(int i = 0; i < 3; i++){ angle[i] = 0; }
  for(int i = 0; i < 3; i++){ acceleration[i] = 0; }
  for(int i = 0; i < 3; i++){ velocity[i] = 0; }
  for(int i = 0; i < 3; i++){ pos[i] = 0; }
  
  timer = millis();
}


void GyroAccel::GetAngleRateInDegrees(int64_t *angles, float *degrees)
{
  for(int i = 0; i < 3; i++)
  {
    degrees[i] = float(angles[i] * 250) / 32768.0;
  }
}

void GyroAccel::GetAccelInMeters(int64_t *accels, float *meters)
{
  for(int i = 0; i < 3; i++)
  {
    meters[i] = (float)accels[i] * 2.0 * 9.81 / 32768.0;
  }
}

void GyroAccel::Update()
{
  long int curr_time = millis();
  dt = curr_time - timer;
  timer = curr_time;
 
  MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro)); 
 
  uint8_t swap;
  #define SWAP(x,y) swap = x; x = y; y = swap
 
  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);
 
  // Compute Motion Tracking
  
  raw_angular_velocity[0] = accel_t_gyro.value.x_gyro;
  raw_angular_velocity[1] = accel_t_gyro.value.y_gyro;
  raw_angular_velocity[2] = accel_t_gyro.value.z_gyro;
  GetAngleRateInDegrees(raw_angular_velocity, angular_velocity);
  angle[0] += angular_velocity[0] * dt / 1000.0;
  angle[1] += angular_velocity[1] * dt / 1000.0;
  angle[2] += angular_velocity[2] * dt / 1000.0;
  
  raw_acceleration[0] = accel_t_gyro.value.x_accel;
  raw_acceleration[1] = accel_t_gyro.value.y_accel;
  raw_acceleration[2] = accel_t_gyro.value.z_accel;
  GetAccelInMeters(raw_acceleration, acceleration);
  pos[0] += velocity[0] * dt / 1000.0;
  pos[1] += velocity[1] * dt / 1000.0;
  pos[2] += velocity[2] * dt / 1000.0;
  velocity[0] += acceleration[0] * dt / 1000.0;
  velocity[1] += acceleration[1] * dt / 1000.0;
  velocity[2] += acceleration[2] * dt / 1000.0;
}
