#ifndef MOTION_H
#define MOTION_H

#include "states.h"
#include "gyro.h"
#include "milleniumencoder.h"

class Motion
{
public:
  GyroAccel *gyro;
  MilleniumEncoder *left_encoder, *right_encoder;
  State *state, *next_state;
  ControlInput *control_input;
  
  Motion(GyroAccel *gyro_in, MilleniumEncoder *left_encoder_in, MilleniumEncoder *right_encoder_in);
  void UpdateState();
};

#endif
