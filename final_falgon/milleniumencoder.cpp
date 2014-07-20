#include "milleniumencoder.h"

MilleniumEncoder::MilleniumEncoder(int pin_a_in, int pin_b_in, bool is_inverted_in)
{
  pin_a = pin_a_in;
  pin_b = pin_b_in;
  pinMode(pin_a, INPUT);
  digitalWrite(pin_a, HIGH);
  pinMode(pin_b, INPUT);
  digitalWrite(pin_b, HIGH);
  
  prev_a = digitalRead(pin_a);
  counts = 0;
  delta_counts = 0;
  is_inverted = is_inverted_in;
  period = 0;
  velocity = 0.0;
  meters = 0.0;
  velocity_meters = 0.0;
  
  max_period = 200;
}

void MilleniumEncoder::Update()
{
  int a = digitalRead(pin_a);
  int b = digitalRead(pin_b);
  delta_counts = 0;
  
  if(a != prev_a)
  {
    // Change
    if(a == b)
    {
      // Moving forward
      delta_counts = 1;
    }
    else
    {
      // Moving backward
      delta_counts = -1;
    }
    
    if(is_inverted)
    {
    delta_counts = -delta_counts;
    }
    
    // TODO: Add in direction change logic
    if(delta_counts * velocity < 0)
    {
      // Direction change: set vel to 0
      velocity = 0.0;
      last_count = millis();
    }
    else
    {
      period = millis() - last_count;
      last_count = millis();
      velocity = 1000.0 / period * delta_counts;
    }
  }
  
  prev_a = a;
  counts += delta_counts;
  
  if(millis() - last_count > max_period){ velocity = 0.0; }
  
  meters = CountsToMeters(counts);
  velocity_meters = CountsToMeters(velocity);
  
}

float MilleniumEncoder::CountsToMeters(int counts)
{
  return ((float)counts) * 2.75 * 3.14 / 48.0 * 2.54 / 100.0;
}
