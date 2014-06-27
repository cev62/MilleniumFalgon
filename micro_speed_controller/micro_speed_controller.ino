#include <SoftwareSerial.h>

int left_motor_forward = 5;
int left_motor_reverse = 6;
int left_motor_direction_forward = 7;
int left_motor_direction_reverse = 8;

int right_motor_forward = 9;
int right_motor_reverse = 10;
int right_motor_direction_forward = 13;
int right_motor_direction_reverse = 12;

float deadzone = 0.1;

const int command_begin = 224;
const int command_end = 192;
const int data_max = 127;
const int data_min = 0;
const int data_length = 1;

int rx = 2;
int tx = 3;
SoftwareSerial softSerial(rx, tx);

float left_percent;
float right_percent;

void SetMotor(int pwm_forward, int pwn_reverse, int direction_forward, int direction_reverse, float percent_power);
void SetDriveLR();
void DecodeCommand(int data[]);


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
  
  left_percent = 0.0;
  right_percent = 0.0;
  SetDriveLR();
  
  softSerial.begin(9600);
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
  
  bool is_received = Serial.available();
  if(is_received)
  {
    bool is_command_begun = false;
    int data = 0;
    int data_index = 0;
    delay(30);
    while(Serial.available() > 0)
    {
      int input = Serial.read();
      if(is_command_begun)
      {
        if(input == command_end)
        {
          DecodeCommand(data);
          state = RUN;
          SetDriveLR();
          watchdog_timer_start = millis();
          is_command_begun = false;
          break;
        }
        data = input;
      }
      is_command_begun = input == command_begin || is_command_begun;
    }
  }  
  
  if(state == NONE)
  {
    left_percent = 0.0;
    right_percent = 0.0;
    SetDriveLR();
  }
  else if (state == RUN)
  {
    SetDriveLR();
  }
  
}

void SetMotor(int pwm_forward, int pwm_reverse, int direction_forward, int direction_reverse, float percent_power)
{
  bool reverse = false;
  float power = min(1.0, max(-1.0, percent_power));
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
      analogWrite(pwm_forward, (int)(255 * power));
    }
    else
    {
      digitalWrite(direction_reverse, HIGH);
      digitalWrite(direction_forward, LOW);
      analogWrite(pwm_reverse, (int)(255 * power));
      analogWrite(pwm_forward, 0);
    }
  }
}

void SetDriveLR()
{
  SetMotor(left_motor_forward, left_motor_reverse, left_motor_direction_forward, left_motor_direction_reverse, left_percent);
  SetMotor(right_motor_forward, right_motor_reverse, right_motor_direction_forward, right_motor_direction_reverse, right_percent);
  Serial.print(left_percent);
  Serial.write(",");
  Serial.println(right_percent);
}


void DecodeCommand(int data)
{  
  int up = data & 8;
  int down = data & 4;
  int left = data & 2;
  int right = data & 1;
  
  //left_percent = up ? 1.0 : 0 + down ? -1.0 : 0 + left ? -1.0 : 0.0 + right ? 1.0 : 0;
  //right_percent = up ? 1.0 : 0 + down ? -1.0 : 0 + left ? 1.0 : 0.0 + right ? -1.0 : 0;
  
  if(up && down){up = 0; down = 0;}
  if(left && right){left = 0; right = 0;}
  if(up && left){left_percent = 0.0; right_percent = 1.0;}
  if(up && right){left_percent = 1.0; right_percent = 0.0;}
  if(down && left){left_percent = -1.0; right_percent = 0.0;}
  if(down && right){left_percent = 0.0; right_percent = -1.0;}
  if(up && !right && !left){left_percent = 1.0; right_percent = 1.0;}
  if(down && !right && !left){left_percent = -1.0; right_percent = -1.0;}
  if(left && !up && !down){left_percent = -1.0; right_percent = 1.0;}
  if(right && !up && !down){left_percent = 1.0; right_percent = -1.0;}
  if(!left && !right && !up && !down){left_percent = 0.0; right_percent = 0.0;}
  
  left_percent = min(max(left_percent, -1.0), 1.0);
  right_percent = min(max(right_percent, -1.0), 1.0);
}

