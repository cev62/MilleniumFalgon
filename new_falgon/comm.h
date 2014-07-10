#ifndef COMM_H
#define COMM_H

#include <SoftwareSerial.h>
#include "Arduino.h"

typedef enum ControlState
{
  DISABLED = 0,
  TELEOP,
  AUTO
} ControlState;

typedef struct Command
{
  int left_power;
  int right_power;
  int arm_angle;
  int box_angle;
  ControlState control_state;
  bool is_fresh_command;
} Command;

typedef struct MicroCommand
{
  int left_power;
  int right_power;
} MicroCommand;

class Comm
{
public:
  Comm();
  void Update(Command *target_command);
  void SendCommandToMicro(MicroCommand *micro_command);
  void EncodeCommand(int *data, Command *target_command);
  int SerialAvailable(int mode);
  int SerialRead(int mode);
  
  SoftwareSerial blue_serial;
  int data_length, command_begin, command_end;
  long int micro_timer;
  int micro_timeout;
};

#endif
