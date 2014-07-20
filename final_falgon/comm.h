#ifndef COMM_H
#define COMM_H

#include <SoftwareSerial.h>
#include "Arduino.h"
#include "states.h"

class Comm
{
public:
  Comm();
  void Update(Command *target_command);
  void SendCommandToMicro(int left_power, int right_power);
  void EncodeCommand(int *data, Command *target_command);
  int SerialAvailable(int mode);
  int SerialRead(int mode);
  
  SoftwareSerial blue_serial;
  int data_length, command_begin, command_end;
  long int micro_timer;
  int micro_timeout;
};

#endif
