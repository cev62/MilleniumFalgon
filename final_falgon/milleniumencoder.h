#ifndef MILLENIUM_ENCODER_H
#define MILLENIUM_ENCODER_H

#include "Arduino.h"

class MilleniumEncoder
{
public:
  MilleniumEncoder(int pin_a_in, int pin_b_in, bool is_inverted_in);
  void Update();
  float CountsToMeters(int counts);
  int counts, delta_counts, pin_a, pin_b, prev_a, period;
  long int last_count;
  float velocity, meters, velocity_meters;
  bool is_inverted;
  int max_period;
};

#endif
