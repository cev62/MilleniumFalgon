#include <SoftwareSerial.h>

int left_motor_forward = 13;
int left_motor_reverse = 11;
int left_motor_direction_forward = 7;
int left_motor_direction_reverse = 8;

int right_motor_forward = 6;
int right_motor_reverse = 5;
int right_motor_direction_forward = 2;
int right_motor_direction_reverse = 3;

float deadzone = 0.1;

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
    int left = Serial.parseInt();
    int right = Serial.parseInt();
    if(Serial.read() == ';')
    {
      left_percent = (float)left / 255;
      right_percent = (float)right / 255;
      state = WIRED;
      blueSerial.println("State: WIRED (received)");
      Serial.println("State: WIRED (received)");
      watchdog_timer_start = millis();
    }
  }
  
  if(state == NONE)
  {
    left_percent = 0.0;
    right_percent = 0.0;
    SetDriveLR();
  }
  else if (state == BLUETOOTH)
  {
    SetDriveLR();
  }
  else if (state == WIRED)
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
  Serial.println("Set Motor");
  Serial.println(left_percent);
  SetMotor(left_motor_forward, left_motor_reverse, left_motor_direction_forward, left_motor_direction_reverse, left_percent);
  SetMotor(right_motor_forward, right_motor_reverse, right_motor_direction_forward, right_motor_direction_reverse, right_percent);
}


void DecodeCommand(int data)
{
  bool up = false, down = false, left = false, right = false;
  int a = data;
  int b = a % 16;
  int c = b % 8;
  int d = c % 4;
  int e = d % 2;
  up = b != c;
  down = c != d;
  left = d != e;
  right = e == 1;
  
  left_percent = up ? 1.0 : 0 + down ? -1.0 : 0 + left ? -1.0 : 0.0 + right ? 1.0 : 0;
  right_percent = up ? 1.0 : 0 + down ? -1.0 : 0 + left ? 1.0 : 0.0 + right ? -1.0 : 0;
  
  left_percent = min(max(left_percent, -1.0), 1.0);
  right_percent = min(max(right_percent, -1.0), 1.0);
  
}
