#include "comm.h"

Comm::Comm():
blue_serial(9, 10)
{
  Serial.begin(9600);
  Serial1.begin(9600);
  
  micro_timer = millis();
  micro_timeout = 50;
  
  data_length = 3;
  command_begin = 224;
  command_end = 192;
}

void Comm::Update(Command *target_command)
{
  for(int mode = 0; mode < 2; mode++)
  {
    while(SerialAvailable(mode) > data_length + 1)
    {
      bool is_command_begun = false;
      int data[data_length];
      int data_index = 0;
      while(SerialAvailable(mode) > 0)
      {
        int input = SerialRead(mode);
        if(is_command_begun)
        {
          if(input == command_end || data_index > data_length)
          {
            EncodeCommand(data, target_command);
            is_command_begun = false;
            break;
          }
          data[data_index] = input;
          data_index++;
        }
        is_command_begun = input == command_begin || is_command_begun;
      }
    }
  }      
}

int Comm::SerialAvailable(int mode)
{
  if(mode == 0){ return Serial.available(); }
  if(mode == 2){ return Serial1.available(); }
  if(mode == 1){ return blue_serial.available(); }
  return 0;
}

int Comm::SerialRead(int mode)
{
  if(mode == 0){ return Serial.read(); }
  if(mode == 2){ return Serial1.read(); }
  if(mode == 1){ return blue_serial.read(); }
  return 0;
}

void Comm::EncodeCommand(int *data, Command *target_command)
{
  int right = data[0] & 1;
  int left = data[0] & 2;
  int down = data[0] & 4;
  int up = data[0] & 8;
  
  target_command->left_power = 0;
  target_command->right_power = 0;
  
  if(up != 0)   { target_command->left_power += 63; target_command->right_power += 63; }
  if(down != 0) { target_command->left_power -= 63; target_command->right_power -= 63; }
  if(left != 0) { target_command->left_power -= 63; target_command->right_power += 63; }
  if(right != 0){ target_command->left_power += 63; target_command->right_power -= 63; }
  
  target_command->left_power = min(63, max(-63, target_command->left_power));
  target_command->right_power = min(63, max(-63, target_command->right_power));
  
  int servo_a = data[1] & 16;
  int servo_b = data[1] & 32;
  
  if (!servo_a && !servo_b) {target_command->arm_angle = -1;}
  if (servo_a && !servo_b) {target_command->arm_angle = 43;}
  if (!servo_a && servo_b) {target_command->arm_angle = 125;}
  if (servo_a && servo_b) {target_command->arm_angle = 170;}
  
  int box_a = data[2] & 16;
  int box_b = data[2] & 32;
  
  if (!box_a && !box_b) {target_command->box_angle = -1;}
  if (box_a && !box_b) {target_command->box_angle = 0;}
  if (!box_a && box_b) {target_command->box_angle = 35;}
  if (box_a && box_b) {target_command->box_angle = 160;}
  
  target_command->csv_output = data[3] != 0;
  
  target_command->control_state = TELEOP;
  
  target_command->is_fresh_command = true;
}

void Comm::SendCommandToMicro(MicroCommand *micro_command)
{
  if(millis() - micro_timer < micro_timeout)
  {
    // Not time to send a new command yet
    return;
  }  
  micro_timer = millis();
  
  byte left_data = abs(micro_command->left_power);
  if(micro_command->left_power < 0) { left_data |= 64; }
  byte right_data = abs(micro_command->right_power);
  if(micro_command->right_power < 0) { right_data |= 64; }
  
  Serial1.write(command_begin);
  Serial1.write(left_data);
  Serial1.write(right_data);
  Serial1.write(command_end);
}
