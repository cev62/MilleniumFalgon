#ifndef ROBOT_H
#define ROBOT_H

#include "comm.h"
#include "gyro.h"
#include "milleniumencoder.h"
#include "Arduino.h"
#include <Servo.h>


typedef struct State
{
public:
  Command *curr_command;
  Command *next_command;
  MicroCommand *micro_command;
  int left_power;
  int right_power;
  int arm_angle;
  int box_angle;
  ControlState control_state;
  bool is_arm_attached;
  bool is_box_attached;
  long int watchdog_timer, state_print_timer;
} State;



class Robot
{
public:
  Robot();
  void UpdateComm();
  void UpdateSensors();
  void Actuate();
  void DecodeCommand(State *state);
  void DisableState();

  State *state;
  Comm *comm;
  GyroAccel *gyro;
  MilleniumEncoder *left_encoder, *right_encoder;
  Servo *arm, *box;
};

#endif
