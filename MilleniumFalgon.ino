#include <SoftwareSerial.h>
#include <Servo.h>

int arm_pin = 4;
int arm_stop_angle = -1;
int arm_store_angle = 43;
int arm_down_angle = 180;
int arm_mid_angle = 135;

const int command_begin = 224;
const int command_end = 192;
const int data_max = 127;
const int data_min = 0;

const int data_length = 3;

int rx_blue = 9;
int tx_blue = 10;
SoftwareSerial blueSerial(rx_blue, tx_blue);

int left_power;
int right_power;

int arm_angle;

Servo arm;

void DecodeCommand(int *data);


typedef enum ControlState
{
  NONE = 0,
  RUN
} ControlState;

ControlState state;
int watchdog_timeout = 200;
unsigned long watchdog_timer_start;
bool blue_handshake = false, wired_handshake = false;

void setup()
{
  
  //arm.attach(arm_pin);
  arm_angle = -1;
  arm.detach();
  
  left_power = 0;
  right_power = 0;
  SetDriveLR();
  
  blueSerial.begin(9600);
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
    blueSerial.println("Watchdog timeout...");
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
  
  if(blueSerial.available())
  {
    bool is_command_begun = false;
    int data[data_length];
    int data_index = 0;
    delay(30);
    while(blueSerial.available() > 0)
    {
      int input = blueSerial.read();
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
  
  if(Serial1.available()){
    Serial.write(Serial1.read());
  }
  
  if(state == NONE)
  {
    left_power = 0;
    right_power = 0;
    arm.detach();
    SetDriveLR();
  }
  else if (state == RUN)
  {
    SetDriveLR();
    if(arm_angle == arm_stop_angle) {arm.detach();}
    else {arm.attach(arm_pin); arm.write(arm_angle);}
  }
  
}

void SetDriveLR()
{
  Serial1.write(command_begin);
  
  byte left_data = abs(left_power);
  if(left_power < 0)
  {
    left_data |= 64;
  }
  byte right_data = abs(right_power);
  if(right_power < 0)
  {
    right_data |= 64;
  }
  
  Serial1.write(left_power);
  Serial1.write(right_power);
  //Serial1.write(command_end);
}


void DecodeCommand(int *data)
{ 
  int right = data[0] & 1;
  int left = data[0] & 2;
  int down = data[0] & 4;
  int up = data[0] & 8;
  
  left_power = 0;
  right_power = 0;
  
  if(up != 0)   { left_power += 63; right_power += 63; }
  if(down != 0) { left_power -= 63; right_power -= 63; }
  if(left != 0) { left_power -= 63; right_power += 63; }
  if(right != 0){ left_power += 63; right_power -= 63; }
  
  left_power = min(63, max(-63, left_power));
  right_power = min(63, max(-63, right_power));
  
  Serial.print("data: ");
  Serial.print(left_power);
  Serial.print(", ");
  Serial.println(right_power);
  
  int servo_a = data[1] & 16;
  int servo_b = data[1] & 32;
  
  if (!servo_a && !servo_b) {arm_angle = arm_stop_angle;}
  if (servo_a && !servo_b) {arm_angle = arm_store_angle;}
  if (!servo_a && servo_b) {arm_angle = arm_mid_angle;}
  if (servo_a && servo_b) {arm_angle = arm_down_angle;}
  
  
}

class PIDController
{
public:
  void Reset();
  PIDController(float p, float i, float d);
  void SetInput(float input_in);
  float GetOutput();
  void SetSetpoint(float setpoint_in);
  void Start();
private:
  float error, prev_error, total_error, kP, kI, kD, input, setpoint, output, dt;
  int timer;
};

PIDController::PIDController(float p, float i, float d)
{
  kP = p;
  kI = i;
  kD = d;
  Reset();
  timer = 0;
}

void PIDController::SetSetpoint(float setpoint_in)
{
  setpoint = setpoint_in;
}

void PIDController::SetInput(float input_in)
{
  input = input_in;
  int new_timer = millis();
  dt = (float)(new_timer - timer) / 1000;
  timer = new_timer;
}

float PIDController::GetOutput()
{
  error = input - setpoint;
  total_error += error * dt;
  float delta_error = (error - prev_error) / dt;
  
  output = kP*error + kI*total_error + kD*delta_error;
  output = min(max(output, -1.0), 1.0);
  
  prev_error = error;
  
  return output;
}

void PIDController::Start()
{
  Reset();
  timer = millis();
}

void PIDController::Reset()
{
  error = 0;
  prev_error = 0;
  total_error = 0;
  input = 0;
  setpoint = 0;
  output = 0;
  timer = millis();
}


