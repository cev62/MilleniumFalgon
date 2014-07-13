#include "robot.h"

Robot::Robot()
{
  state = new State;
  state->curr_command = new Command;
  state->next_command = new Command;
  state->micro_command = new MicroCommand;
  DisableState();
  
  state->is_arm_attached = false;
  state->is_box_attached = false;
  state->csv_output = false;
  
  gyro = new GyroAccel();
  gyro->Reset();
  
  left_encoder = new MilleniumEncoder(11, 12, false);
  right_encoder = new MilleniumEncoder(6, 7, true);
  
  arm = new Servo;
  box = new Servo;
  arm->detach();
  box->detach();
  
  comm = new Comm();
}

void Robot::UpdateComm()
{
  comm->Update(state->next_command);
  
  // check if fresh, etc.
  if(state->next_command->is_fresh_command)
  {
    DecodeCommand(state);
    state->watchdog_timer = millis();
  }
  
  //Check if the watchdog timer has failed
  if(millis() - state->watchdog_timer > 200 && state->control_state != DISABLED)
  {
    state->control_state = DISABLED;
    Serial.println("Watchdog timeout...");
  }
  
  if(millis() - state->state_print_timer > 20)
  {
    if(!state->csv_output)
    {
      Serial.println();
      Serial.println("Millenium Falgon State: ");
      Serial.print("Drive power: ");
      Serial.print(state->left_power);
      Serial.print(", ");
      Serial.println(state->right_power);
      Serial.print("Arm angle: ");
      Serial.println(state->arm_angle);
      Serial.print("Box angle: ");
      Serial.println(state->box_angle);
      Serial.print("Gyro: ");
      Serial.println(gyro->angle[2]);
      Serial.print("Encoder: ");
      Serial.print(left_encoder->counts);
      Serial.print(", ");
      Serial.println(right_encoder->counts);
      Serial.print("Encoder Vel: ");
      Serial.print(left_encoder->velocity);
      Serial.print(", ");
      Serial.println(right_encoder->velocity);
    }
    else
    {
      Serial.print(millis());
      Serial.print(",");
      Serial.print(state->left_power);
      Serial.print(",");
      Serial.print(state->right_power);
      Serial.print(",");
      Serial.print(state->arm_angle);
      Serial.print(",");
      Serial.print(state->box_angle);
      Serial.print(",");
      Serial.print(gyro->angle[2]);
      Serial.print(",");
      Serial.print(gyro->velocity[1]);
      Serial.print(",");
      Serial.print(left_encoder->counts);
      Serial.print(",");
      Serial.print(right_encoder->counts);
      Serial.print(",");
      Serial.print(left_encoder->velocity);
      Serial.print(",");
      Serial.println(right_encoder->velocity);
    }
    state->state_print_timer = millis();
  }
}

void Robot::UpdateSensors()
{
  gyro->Update();
  left_encoder->Update();
  right_encoder->Update();
}

void Robot::Actuate()
{
  if(state->control_state == DISABLED)
  {
    DisableState();
  }
  
  if(state->control_state == TELEOP)
  {
    // Actuate Micro Speed Controller
    state->micro_command->left_power = state->left_power;
    state->micro_command->right_power = state->right_power;
    comm->SendCommandToMicro(state->micro_command);
    
    if(state->arm_angle == -1 && state->is_arm_attached)
    {
      state->is_arm_attached = false;
      arm->detach();
    }
    if(state->box_angle == -1 && state->is_box_attached)
    {
      state->is_box_attached = false;
      box->detach();
    }
    if(state->arm_angle != -1)
    {
      if(!state->is_arm_attached)
      {
        arm->attach(4, 500, 2500);
        state->is_arm_attached = true;
      }
      arm->write(state->arm_angle);
    }
    if(state->box_angle != -1)
    {
      if(!state->is_box_attached)
      {
        box->attach(5, 500, 2500);
        state->is_box_attached = true;
      }
      box->write(state->box_angle);
    }
  }
  
  if(state->control_state == AUTO)
  {
    
  }
  
}

void Robot::DecodeCommand(State *state)
{
  // Dump contents of the new command into the current command
  state->curr_command->left_power = state->next_command->left_power;
  state->curr_command->right_power = state->next_command->right_power;
  state->curr_command->arm_angle = state->next_command->arm_angle;
  state->curr_command->box_angle = state->next_command->box_angle;
  state->curr_command->control_state = state->next_command->control_state;
  state->curr_command->csv_output = state->next_command->csv_output;
  state->next_command->is_fresh_command = false;
  
  // Update the state based on the new current command
  state->left_power = state->curr_command->left_power;
  state->right_power = state->curr_command->right_power;
  state->arm_angle = state->curr_command->arm_angle;
  state->box_angle = state->curr_command->box_angle;
  state->control_state = state->curr_command->control_state;
  state->csv_output = state->curr_command->csv_output;
}

void Robot::DisableState()
{
  state->left_power = 0;
  state->right_power = 0;
  state->arm_angle = -1;
  state->box_angle = -1;
  state->control_state = DISABLED;
  state->watchdog_timer = millis();
}
