#include <SoftwareSerial.h>
#include <Servo.h>

int left_motor_forward = 13;
int left_motor_reverse = 11;
int left_motor_direction_forward = 7;
int left_motor_direction_reverse = 8;

int right_motor_forward = 6;
int right_motor_reverse = 5;
int right_motor_direction_forward = 2;
int right_motor_direction_reverse = 3;

float deadzone = 0.1;

int arm_pin = 4;
int arm_stop_angle = -1;
int arm_store_angle = 43;
int arm_down_angle = 180;
int arm_mid_angle = 135;

const int handshake_response_bluetooth = 240;
const int handshake_response_wired = 208;
const int handshake_begin = 224;
const int handshake_end = 192;
const int command_begin = 224;
const int command_end = 192;
const int data_max = 127;
const int data_min = 0;

const int data_length = 1;
int password[] = {1, 2, 3, 4}; // Must be 4 digits right now

int rx = 9;
int tx = 10;
SoftwareSerial blueSerial(rx, tx);

float left_percent;
float right_percent;

int arm_angle;

Servo arm;

void SetMotor(int pwm_forward, int pwn_reverse, int direction_forward, int direction_reverse, float percent_power);
void SetDriveLR();
void DecodeCommand(int data[]);


typedef enum ControlState
{
  NONE = 0,
  BLUETOOTH,
  WIRED
} ControlState;

ControlState state;
int watchdog_timeout = 150;
unsigned long watchdog_timer_start;
bool blue_handshake = false, wired_handshake = false;

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
  
  arm.attach(arm_pin);
  arm_angle = -1;
  arm.detach();
  
  left_percent = 0.0;
  right_percent = 0.0;
  SetDriveLR();
  
  blueSerial.begin(9600);
  Serial.begin(9600);
  
  state = NONE;
  watchdog_timer_start = millis();
}

void loop()
{
  if(millis() - watchdog_timer_start > watchdog_timeout && (state == BLUETOOTH || state == WIRED))
  {
    // Watchdog timed out: revert to control state NONE
    state = NONE;
    Serial.println("Watchdog timeout...");
    blueSerial.println("Watchdog timeout...");
  }  
  
  bool is_bluetooth_received = blueSerial.available();
  if(is_bluetooth_received)
  {
    if(blue_handshake)
    {
      //Serial.println("Command");
      bool is_command_begun = false;
      int data = 0;
      int data_index = 0;
      delay(30);
      while(blueSerial.available() > 0)
      {
        int input = blueSerial.read();
        if(is_command_begun)
        {
          if(input == command_end)
          {
            DecodeCommand(data);
            
            state = BLUETOOTH;
            blueSerial.println("State: BLUETOOTH (received)");
            SetDriveLR();
            //Serial.println("State: BLUETOOTH (received)");
            //Serial.println("DDD");
            //Serial.println(data);
            watchdog_timer_start = millis();
            is_command_begun = false;
            break;
          }
          data = input;
        }
        is_command_begun = input == command_begin || is_command_begun;
      }
    }
    else
    {
      bool is_handshake_begun = false;
      int password_index = 0;
      bool is_password_passed = true;
      while(blueSerial.available() > 0)
      {
        int input = blueSerial.read();
        if(is_handshake_begun)
        {
          Serial.println("begin");
          if(input == handshake_end)
          {
            Serial.println("end");
            blueSerial.write(handshake_response_bluetooth);//handshake_response_bluetooth);
            Serial.write(handshake_response_bluetooth);//handshake_response_bluetooth);
            blue_handshake = true;
            break;
            /*if(password_index < 3){ is_password_passed = false;}
            if(is_password_passed)
            {
              blueSerial.write(handshake_response_bluetooth);
              blue_handshake = true;
              Serial.println("pass");
            }
            else
            {
              blueSerial.write((byte)0);
              Serial.println("fail");
            }*/
          }
          //if(input != password[password_index]){ is_password_passed = false; }
          //password_index++;
        }
        is_handshake_begun = input == handshake_begin || is_handshake_begun;
      }
    }
  }  
  
  bool is_wired_received = Serial.available();
  if(is_wired_received)
  {
    if(wired_handshake)
    {
      //Serial.println("Command");
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
            
            state = WIRED;
            Serial.println("State: WIRED (received)");
            SetDriveLR();
            //Serial.println("State: BLUETOOTH (received)");
            //Serial.println("DDD");
            //Serial.println(data);
            watchdog_timer_start = millis();
            is_command_begun = false;
            break;
          }
          data = input;
        }
        is_command_begun = input == command_begin || is_command_begun;
      }
    }
    else
    {
      bool is_handshake_begun = false;
      int password_index = 0;
      bool is_password_passed = true;
      while(Serial.available() > 0)
      {
        int input = Serial.read();
        if(is_handshake_begun)
        {
          Serial.println("begin");
          if(input == handshake_end)
          {
            Serial.println("end");
            blueSerial.write(handshake_response_wired);//handshake_response_bluetooth);
            Serial.write(handshake_response_wired);//handshake_response_bluetooth);
            wired_handshake = true;
            break;
            /*if(password_index < 3){ is_password_passed = false;}
            if(is_password_passed)
            {
              blueSerial.write(handshake_response_bluetooth);
              blue_handshake = true;
              Serial.println("pass");
            }
            else
            {
              blueSerial.write((byte)0);
              Serial.println("fail");
            }*/
          }
          //if(input != password[password_index]){ is_password_passed = false; }
          //password_index++;
        }
        is_handshake_begun = input == handshake_begin || is_handshake_begun;
      }
    }
    
    /*int left = Serial.parseInt();
    int right = Serial.parseInt();
    if(Serial.read() == ';')
    {
      left_percent = (float)left / 255;
      right_percent = (float)right / 255;
      state = WIRED;
      blueSerial.println("State: WIRED (received)");
      Serial.println("State: WIRED (received)");
      watchdog_timer_start = millis();
    }*/
  }
  
  if(state == NONE)
  {
    left_percent = 0.0;
    right_percent = 0.0;
    arm.detach();
    SetDriveLR();
  }
  else if (state == BLUETOOTH)
  {
    SetDriveLR();
    if(arm_angle == arm_stop_angle) {arm.detach();}
    else {arm.attach(arm_pin); arm.write(arm_angle);}
  }
  else if (state == WIRED)
  {
    SetDriveLR();
    if(arm_angle == -1) {arm.detach();}
    else {arm.attach(arm_pin); arm.write(arm_angle);}
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
  //Serial.println("Set Motor");
  //Serial.println(left_percent);
  SetMotor(left_motor_forward, left_motor_reverse, left_motor_direction_forward, left_motor_direction_reverse, left_percent);
  SetMotor(right_motor_forward, right_motor_reverse, right_motor_direction_forward, right_motor_direction_reverse, right_percent);
}


void DecodeCommand(int data)
{
  /*bool up = false, down = false, left = false, right = false;
  int a = data;
  int b = a % 16;
  int c = b % 8;
  int d = c % 4;
  int e = d % 2;
  up = b != c;
  down = c != d;
  left = d != e;
  right = e == 1;*/
  
  int up = data & 8;
  int down = data & 4;
  int left = data & 2;
  int right = data & 1;
  
  int servo_a = data & 16;
  int servo_b = data & 32;
  
  if (!servo_a && !servo_b) {arm_angle = arm_stop_angle;}
  if (servo_a && !servo_b) {arm_angle = arm_store_angle;}
  if (!servo_a && servo_b) {arm_angle = arm_mid_angle;}
  if (servo_a && servo_b) {arm_angle = arm_down_angle;}
  
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
  
  /*if(up){Serial.write('u');}
  if(down){Serial.write('d');}
  if(left){Serial.write('l');}
  if(right){Serial.write('r');}
  if(servo_a){Serial.write('a');}
  if(servo_b){Serial.write('b');}
  Serial.write(arm_angle);
  Serial.write('\n');*/
  
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


