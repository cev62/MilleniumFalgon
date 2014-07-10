
#include "robot.h"
#include "comm.h"
#include "gyro.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h>

Robot *robot;

void setup()
{
  robot = new Robot();
}

void loop()
{
  robot->UpdateComm();
  robot->UpdateSensors();
  robot->Actuate();
  delay(10);
}
