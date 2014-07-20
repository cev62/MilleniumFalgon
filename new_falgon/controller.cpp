#include "controller.h"

Controller::Controller(ControlInputs *control_inputs, Sensors *sensors, State *state, Command *command)
{
  control_inputs = control_inputs_in;
  sensors = sensors_in;
  state = state_in;
  command = command_in;
}

TeleopController::Update()
{
  if(command->is_fresh)
  {
    control_inputs->left_power = command->left_power;
    control_inputs->right_power = command->right_power;
    control_inputs->arm_angle = command->arm_angle;
    control_inputs->box_angle = command->box->angle;
  }
}
