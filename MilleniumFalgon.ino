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

int rx = 9;
int tx = 10;
SoftwareSerial blueSerial(rx, tx);

float left_percent;
float right_percent;

void SetMotor(int pwm_forward, int pwn_reverse, int direction_forward, int direction_reverse, float percent_power);
void SetDriveLR();

typedef enum ControlState
{
  NONE = 0,
  BLUETOOTH,
  WIRED
} ControlState;

ControlState state;
int watchdog_timeout = 1000;
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
    int left = blueSerial.parseInt();
    int right = blueSerial.parseInt();
    if(blueSerial.read() == ';')
    {
      left_percent = (float)left / 255;
      right_percent = (float)right / 255;
      state = BLUETOOTH;
      blueSerial.println("State: BLUETOOTH (received)");
      Serial.println("State: BLUETOOTH (received)");
      watchdog_timer_start = millis();
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
  SetMotor(left_motor_forward, left_motor_reverse, left_motor_direction_forward, left_motor_direction_reverse, left_percent);
  SetMotor(right_motor_forward, right_motor_reverse, right_motor_direction_forward, right_motor_direction_reverse, right_percent);
}
