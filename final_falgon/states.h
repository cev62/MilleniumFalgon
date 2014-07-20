#ifndef STATES_H
#define STATES_H

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

#endif
