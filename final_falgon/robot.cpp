#include "robot.h"

Robot::Robot()
{
  state = new State;
  state->pos = new float[3];
  state->velocity = new float[3];
  state->acceleration = new float[3];
  state->angle = new float[3];
  state->angular_velocity = new float[3];
  
  control_input = new ControlInput;
  control_input->left_power = 0;
  control_input->right_power = 0;
  control_input->arm_angle = -1;
  control_input->arm_angle = -1;
  
  command = new Command;
  command->control_state = DISABLED;
  command->is_fresh_command = false;
  command->csv_output = false;
  command->left_power = 0;
  command->right_power = 0;
  command->arm_angle = -1;
  command->box_angle = -1;
  
  gyro = new GyroAccel();
  gyro->Reset();
  left_encoder = new MilleniumEncoder(11, 12, false);
  right_encoder = new MilleniumEncoder(6, 7, true);
  
  arm = new Servo;
  box = new Servo;
  arm->detach();
  box->detach();
  is_arm_attached = false;
  is_box_attached = false;
  
  comm = new Comm();
  csv_output = false;
  
  //curr_controller = new TeleopController(control_input, state, command);
  
  DisableInputs();
}

void Robot::UpdateComm()
{
  comm->Update(command);
  
  if(command->is_fresh_command)
  {
    watchdog_timer = millis();
  }
  
  //Check if the watchdog timer has failed
  if(millis() - watchdog_timer > 200 && control_state != DISABLED)
  {
    DisableInputs();
    Serial.println("Watchdog timeout...");
  }
  
  if(millis() - state_print_timer > 200)
  {
    if(!csv_output)
    {
      /*Serial.println();
      Serial.println("Millenium Falgon State: ");
      Serial.print("Drive power: ");
      Serial.print(command->left_power);
      Serial.print(", ");
      Serial.println(control_input->right_power);
      Serial.print("Arm angle: ");
      Serial.println(control_input->arm_angle);
      Serial.print("Box angle: ");
      Serial.println(control_input->box_angle);
      Serial.print("Gyro: ");
      Serial.println(gyro->angle[2]);
      Serial.print("Encoder: ");
      Serial.print(left_encoder->meters);
      Serial.print(", ");
      Serial.println(right_encoder->meters);
      Serial.print("Encoder Vel: ");
      Serial.print(left_encoder->velocity);
      Serial.print(", ");
      Serial.println(right_encoder->velocity);
      Serial.print("Encoder Vel Meters: ");
      Serial.print(left_encoder->velocity_meters);
      Serial.print(", ");
      Serial.println(right_encoder->velocity_meters);*/
    }
    else
    {
      /*Serial.print(millis());
      Serial.print(",");
      Serial.print(command->left_power);
      Serial.print(",");
      Serial.print(control_input->right_power);
      Serial.print(",");
      Serial.print(control_input->arm_angle);
      Serial.print(",");
      Serial.print(control_input->box_angle);
      Serial.print(",");
      Serial.print(gyro->angle[2]);
      Serial.print(",");
      Serial.print(gyro->velocity[1]);
      Serial.print(",");
      Serial.print(left_encoder->meters);
      Serial.print(",");
      Serial.print(right_encoder->meters);
      Serial.print(",");
      Serial.print(left_encoder->velocity);
      Serial.print(",");
      Serial.print(right_encoder->velocity);
      Serial.print(",");
      Serial.print(left_encoder->velocity_meters);
      Serial.print(",");
      Serial.println(right_encoder->velocity_meters);*/
    }
    state_print_timer = millis();
  }
}

void Robot::UpdateSensors()
{
  gyro->Update();
  left_encoder->Update();
  right_encoder->Update();
}

// Updates the current controller based on the new command (if fresh) and sensor values
// Then sets contorl_inputs based on the output of the controller
void Robot::GenerateInputs()
{
  control_state = command->control_state;
  control_input->left_power = command->left_power;
  control_input->right_power = command->right_power;
  control_input->arm_angle = command->arm_angle;
  control_input->box_angle = command->box_angle;
  //curr_controller->Update();
}

void Robot::Actuate()
{
  if(control_state == DISABLED)
  {
    DisableInputs();
  }
  else
  {
    // Actuate Micro Speed Controller
    comm->SendCommandToMicro(control_input->left_power, control_input->right_power);
    
    if(control_input->arm_angle == -1 && is_arm_attached)
    {
      is_arm_attached = false;
      arm->detach();
    }
    if(control_input->box_angle == -1 && is_box_attached)
    {
      is_box_attached = false;
      box->detach();
    }
    if(control_input->arm_angle != -1)
    {
      if(!is_arm_attached)
      {
        arm->attach(4, 500, 2500);
        is_arm_attached = true;
      }
      arm->write(control_input->arm_angle);
    }
    if(control_input->box_angle != -1)
    {
      if(!is_box_attached)
      {
        box->attach(5, 500, 2500);
        is_box_attached = true;
      }
      box->write(control_input->box_angle);
    }
  }  
}

void Robot::DisableInputs()
{
  control_state = DISABLED;
  control_input->left_power = 0;
  control_input->right_power = 0;
  control_input->arm_angle = -1;
  control_input->arm_angle = -1;  
}
