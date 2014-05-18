#include <SoftwareSerial.h>

int left_motor_forward = 13;
int left_motor_reverse = 11;
int left_motor_direction_forward = 8;
int left_motor_direction_reverse = 7;

int right_motor_forward = 6;
int right_motor_reverse = 5;
int right_motor_direction_forward = 3;
int right_motor_direction_reverse = 2;

int rx = 9;
int tx = 10;
SoftwareSerial blueSerial(rx, tx);

float left_percent = 0.0;
float right_percent = 0.0;

void SetDriveLR(float left_percent, float right_percent);

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
  blueSerial.begin(9600);
  Serial.begin(9600);
  SetDriveLR(0.0, 0.0);
}

void loop()
{
  while(blueSerial.available() > 0)
  {
    int left = blueSerial.parseInt();
    int right = blueSerial.parseInt();
    
    if(blueSerial.read() == ';')
    {
      blueSerial.print("Received");
      SetDriveLR((float)left / 255, (float)right / 255);
    }
  }
}

void SetDriveLR(float left_percent_in, float right_percent_in)
{
  float left_percent = left_percent_in, right_percent = right_percent_in;
  boolean left_reverse = false, right_reverse = false;
  
  // arguments are on [-1.0, 1.0] range
  left_percent = min(1.0, max(-1.0, left_percent));
  right_percent = min(1.0, max(-1.0, right_percent));
  if(left_percent < 0)
  {
    left_percent = -left_percent;
    left_reverse = true;
  }
  if(right_percent < 0)
  {
    right_percent = -right_percent;
    right_reverse = true;
  }
  
  if(left_percent == 0.0)
  {
    digitalWrite(left_motor_direction_reverse, LOW);
    digitalWrite(left_motor_direction_forward, LOW);
    analogWrite(left_motor_reverse, (int)(0));
    analogWrite(left_motor_forward, (int)(0));
  }
  else
  {
    // Actuate!
    if(!left_reverse)
    {
      digitalWrite(left_motor_direction_reverse, HIGH);
      digitalWrite(left_motor_direction_forward, LOW);
      analogWrite(left_motor_reverse, (int)(0));
      analogWrite(left_motor_forward, (int)(255 * left_percent));
    }
    else
    {
      digitalWrite(left_motor_direction_reverse, LOW);
      digitalWrite(left_motor_direction_forward, HIGH);
      analogWrite(left_motor_reverse, (int)(255 * left_percent));
      analogWrite(left_motor_forward, (int)(0));
    }
  }
  
  if(right_percent == 0.0)
  {
    digitalWrite(right_motor_direction_reverse, LOW);
    digitalWrite(right_motor_direction_forward, LOW);
    analogWrite(right_motor_reverse, (int)(0));
    analogWrite(right_motor_forward, (int)(0));
  }
  else
  {
    if(!right_reverse)
    {
      digitalWrite(right_motor_direction_reverse, HIGH);
      digitalWrite(right_motor_direction_forward, LOW);
      analogWrite(right_motor_reverse, (int)(0));
      analogWrite(right_motor_forward, (int)(255 * right_percent));
    }
    else
    {
      digitalWrite(right_motor_direction_reverse, LOW);
      digitalWrite(right_motor_direction_forward, HIGH);
      analogWrite(right_motor_reverse, (int)(255 * right_percent));
      analogWrite(right_motor_forward, (int)(0));
    }
  }
  
  Serial.write((int)(10 * left_percent_in) + 48);
  Serial.write((int)(10 * right_percent_in) + 48);
  blueSerial.write((int)(10 * left_percent_in) + 48);
  blueSerial.write((int)(10 * right_percent_in) + 48);
}
