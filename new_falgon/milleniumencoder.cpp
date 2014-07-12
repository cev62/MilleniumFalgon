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
  }
  if(is_inverted)
  {
    delta_counts = -delta_counts;
  }
  prev_a = a;
  counts += delta_counts;
}
