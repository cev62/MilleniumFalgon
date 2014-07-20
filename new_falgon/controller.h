#ifndef CONTROLLER_H
#define CONTROLER_H

#include "gyro.h"
#include "milleniumEncoder.h"

typedef enum ControlState
{
  DISABLED = 0,
  TELEOP,
  AUTO
} ControlState;

typedef struct ControlInput
{
  int left_power, right_power, arm_angle, box_angle;
} ControlInput;

typedef struct Sensors
{
  GyroAccel *gyro;
  MilleniumEncoder *left_encoder, *right_encoder;
} Sensors;

typedef struct State
{
public:
  float *pos, *velocity, *acceleration;
  float *angle, *angular_velocity;  
} State;

typedef struct Command
{
  int left_power;
  int right_power;
  int arm_angle;
  int box_angle;
  bool csv_output;
  ControlState control_state;
  bool is_fresh_command;
} Command;

class Controller
{
public:
  ControlInput *control_input;
  Sensors *sensors;
  State *state;
  Command *command;
  
  Controller(ControlInput *control_input_in, Sensors *sensors_in, State *state_in, Command *command_in);
  virtual void Update();
};

class TeleopController : public Controller
{
public:
  TeleopController(ControlInput *control_input_in, Sensors *sensors_in, State *state_in, Command *command_in) :
      Controller::Controller(control_input, sensors, state, command){};
  void Update();
}


#endif
