#ifndef MILLENIUM_ENCODER_H
#define MILLENIUM_ENCODER_H

#include "Arduino.h"

class MilleniumEncoder
{
public:
  MilleniumEncoder(int pin_a_in, int pin_b_in, bool is_inverted_in);
  void Update();
  int counts, delta_counts, pin_a, pin_b, prev_a;
  bool is_inverted;
};

#endif
