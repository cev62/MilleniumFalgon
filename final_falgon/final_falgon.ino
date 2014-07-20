#include "controller.h"
#include "robot.h"
#include "comm.h"
#include "gyro.h"
#include "states.h"
#include "milleniumencoder.h"
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
  //robot->command->left_power = 0;
  Serial.println(robot->command->left_power);
  robot->UpdateComm();
  //robot->UpdateSensors();
  //robot->GenerateInputs();
  //robot->Actuate();
  delay(1);
}
