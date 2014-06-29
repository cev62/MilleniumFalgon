#include <SoftwareSerial.h>

int left_motor_forward = 5;
int left_motor_reverse = 6;
int left_motor_direction_forward = 7;
int left_motor_direction_reverse = 8;

int right_motor_forward = 9;
int right_motor_reverse = 10;
int right_motor_direction_forward = 11;
int right_motor_direction_reverse = 12;

float deadzone = 0.1;

const int command_begin = 224;
const int command_end = 192;
const int data_max = 127;
const int data_min = 0;
const int data_length = 2;

int left_power;
int right_power;

void SetMotor(int pwm_forward, int pwm_reverse, int direction_forward, int direction_reverse, int power);
void SetDriveLR();
void DecodeCommand(int *data);


typedef enum ControlState
{
  NONE = 0,
  RUN,
} ControlState;

ControlState state;
int watchdog_timeout = 200;
unsigned long watchdog_timer_start;

void setup()
{
  pinMode(left_motor_forward, OUTPUT);
  pinMode(left_motor_reverse, OUTPUT);
  pinMode(left_motor_direction_forward, OUTPUT);
  pinMode(left_motor_direction_reverse, OUTPUT);
  pinMode(right_motor_forward, OUTPUT);
  pinMode(right_motor_reverse, OUTPUT);
  pinMode(right_motor_direction_forward, OUTPUT);
  pinMode(right_motor_direction_reverse, OUTPUT);
  
  pinMode(13, OUTPUT);
  
  left_power = 0;
  right_power = 0;
  SetDriveLR();
  
  Serial1.begin(9600);
  Serial.begin(9600);
  
  state = NONE;
  watchdog_timer_start = millis();
}

void loop()
{
  if(millis() - watchdog_timer_start > watchdog_timeout && state == RUN)
  {
    // Watchdog timed out: revert to control state NONE
    state = NONE;
    Serial.println("Watchdog timeout...");
  }
  if(Serial.available())
  {
    bool is_command_begun = false;
    int data[data_length];
    int data_index = 0;
    delay(30);
    while(Serial.available() > 0)
    {
      int input = Serial.read();
      if(is_command_begun)
      {
        if(input == command_end || data_index > data_length)
        {
          DecodeCommand(data);
          state = RUN;
          SetDriveLR();
          watchdog_timer_start = millis();
          is_command_begun = false;
          break;
        }
        data[data_index] = input;
        data_index++;
      }
      is_command_begun = input == command_begin || is_command_begun;
    }
  }    
  
  if(Serial1.available())
  {
    bool is_command_begun = false;
    int data[data_length];
    int data_index = 0;
    delay(10);
    while(Serial1.available() > 0)
    {
      int input = Serial1.read();
      
      if(input == command_begin)
      {
        is_command_begun = true;
        continue;
      }
      
      if(is_command_begun)
      {
        data[data_index++] = input;
        
        if(data_index == data_length)
        {
          DecodeCommand(data);
          state = RUN;
          SetDriveLR();
          watchdog_timer_start = millis();
          is_command_begun = false;
          break;
        }
        if(data_index > data_length)
        {
          // Bad command
          Serial.println("Bad command");
          break;
        }
      }
      is_command_begun = input == command_begin || is_command_begun;
    }
  }    
  
  if(state == NONE)
  {
    left_power = 0;
    right_power = 0;
    SetDriveLR();
  }
  else if (state == RUN)
  {
    SetDriveLR();
  }
}

void SetMotor(int pwm_forward, int pwm_reverse, int direction_forward, int direction_reverse, int power_in)
{
  bool reverse = false;
  int power = min(63, max(-64, power_in));
  if(power < 0)
  {
    power = -power;
    reverse = true;
  }
  
  if(power < deadzone && power > -deadzone)
  {
    digitalWrite(direction_reverse, LOW);
    digitalWrite(direction_forward, LOW);
    analogWrite(pwm_reverse, 0);
    analogWrite(pwm_forward, 0);
  }
  else
  {
    if(!reverse)
    {
      digitalWrite(direction_reverse, LOW);
      digitalWrite(direction_forward, HIGH);
      analogWrite(pwm_reverse, 0);
      analogWrite(pwm_forward, 4 * power);
    }
    else
    {
      digitalWrite(direction_reverse, HIGH);
      digitalWrite(direction_forward, LOW);
      analogWrite(pwm_reverse, 4 * power);
      analogWrite(pwm_forward, 0);
    }
  }
}

void SetDriveLR()
{
  SetMotor(left_motor_forward, left_motor_reverse, left_motor_direction_forward, left_motor_direction_reverse, left_power);
  SetMotor(right_motor_forward, right_motor_reverse, right_motor_direction_forward, right_motor_direction_reverse, right_power);
  
}


void DecodeCommand(int *data)
{  
  // [-64,63]
  left_power = data[0] & 127;
  if(left_power & 64){ left_power = -(left_power - 64); }
  right_power = data[1] & 127;
  if(right_power & 64){ right_power = -(right_power - 64); }
  Serial.print("Data: ");
  Serial.print(left_power);
  Serial.print(", ");
  Serial.println(right_power);
}

