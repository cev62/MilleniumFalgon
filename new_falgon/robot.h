#ifndef ROBOT_H
#define ROBOT_H

#include "comm.h"
#include "gyro.h"
#include "controller."
#include "milleniumencoder.h"
#include "Arduino.h"
#include <Servo.h>



class Robot
{
public:
  Robot();
  void UpdateComm();
  void UpdateSensors();
  void Actuate();
  void GenerateInputs(); // Takes command and controllers into account
  void DisableInputs();

  State *state;
  Sensors *sensors;
  ControlInput *control_input;
  Command *command;
  
  Servo *arm, *box;
  
  Comm *comm;
  
  ControlState control_state;
  
  bool is_arm_attached;
  bool is_box_attached;
  bool csv_output;
  long int watchdog_timer, state_print_timer;
};

#endif
