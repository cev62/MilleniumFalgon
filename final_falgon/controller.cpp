#include "controller.h"

Controller::Controller(ControlInput *control_input_in, State *state_in, Command *command_in)
{
  control_input = control_input_in;
  state = state_in;
  command = command_in;
}

void TeleopController::Update()
{
  if(command->is_fresh_command)
  {
    control_input->left_power = command->left_power;
    control_input->right_power = command->right_power;
    control_input->arm_angle = command->arm_angle;
    control_input->box_angle = command->box_angle;
  }
}
