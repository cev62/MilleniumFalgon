#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "states.h"

class Controller
{
public:
  ControlInput *control_input;
  State *state;
  Command *command;
  
  Controller(ControlInput *control_input_in, State *state_in, Command *command_in);
  void Update(){};
};

class TeleopController : public Controller
{
public:
  TeleopController(ControlInput *control_input_in, State *state_in, Command *command_in) :
      Controller::Controller(control_input, state, command){};
  void Update();
};


#endif
