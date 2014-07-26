import processing.serial.*;

final int command_begin = 224;
final int command_end = 192;
final int data_max = 127;
final int data_min = 0;

Serial serial;
Command command;
boolean up = false, down = false, left = false, right = false, key_change = false;
int arm_state = 0;
int box_state = 0;
int timer;
boolean csv_output = true, reset_gyro = false;

void setup()
{
 println(Serial.list());
 serial = new Serial(this, /*"/dev/ttyACM1"*/ "COM14", 9600);
 command = new Command();
 timer = millis();
}

void draw()
{
  if(key_change)
  {
    command.up = up;
    command.down = down;
    command.left = left;
    command.right = right;
    command.arm = arm_state * 16;
    command.box = box_state * 16;
    command.csv_output = csv_output;
    sendCommand(serial, command);
    //println("(" + up + ", " + down + ", " + left + ", " + right + ")");
    key_change = false;
    reset_gyro = false;
  }
  if(millis() - timer > 50)
  {
    sendCommand(serial, command);
    timer = millis();
  }
  if(serial.available() > 0)
  {
    while(serial.available() > 0)
    {
      print((char)serial.read());
      //serial.read();
    }
    //println();
  }
}

void keyPressed()
{
  if(key == CODED)
  {
    if(keyCode == UP && !up){ up = true; key_change = true; }
    if(keyCode == DOWN && !down){ down = true; key_change = true; }
    if(keyCode == LEFT && !left){ left = true; key_change = true; }
    if(keyCode == RIGHT && !right){ right = true; key_change = true; }
  }
  if(key == 'a'){arm_state = 0; key_change = true; }
  if(key == 's'){arm_state = 1; key_change = true; }
  if(key == 'd'){arm_state = 2; key_change = true; }
  if(key == 'f'){arm_state = 3; key_change = true; }
  
  if(key == 'z'){box_state = 0; key_change = true; }
  if(key == 'x'){box_state = 1; key_change = true; }
  if(key == 'c'){box_state = 2; key_change = true; }
  if(key == 'v'){box_state = 3; key_change = true; }
  
  if(key == 'q'){csv_output = false; key_change = true; }
  if(key == 'w'){csv_output = true; key_change = true; }
  if(key == 'r'){reset_gyro = true; key_change = true; }
}

void keyReleased()
{
  key_change = true;
  if(key == CODED)
  {
    if(keyCode == UP){ up = false; }
    if(keyCode == DOWN) { down = false; }
    if(keyCode == LEFT) { left = false; }
    if(keyCode == RIGHT) { right = false; }
  }
}

private class Command
{
  // Fields contain the information of the command
  boolean up, down, left, right, csv_output, reset_gyro;
  int arm, box;
  Command()
  {
    up = false;
    down = false;
    left = false;
    right = false;
    arm = 0;
    box = 0;
    csv_output = false;
    reset_gyro = false;
  }
  
  /*
   * Exports the command to a series of bytes for transmission
   */
  int[] getData()
  {
    int[] data = new int[4];
    // Puts the booleans into the 4 least significant digits of a single byte
    //data = (right ? 1 : 0) + (left ? 2 : 0) + (down ? 4 : 0) + (up ? 8 : 0) + arm;
    //println(data);
    data[0] = (right ? 1 : 0) + (left ? 2 : 0) + (down ? 4 : 0) + (up ? 8 : 0);
    data[1] = arm;
    data[2] = box;
    data[3] = (csv_output ? 1 : 0) + (reset_gyro ? 2 : 0);
    return data;
  }
}

/*
 * Returns a byte that is cleared to be sent to the robot
 *
 * @ensures output will be read by robot as data
 */
int sanitizeByte(int input)
{
  int output = input;
  if(output > data_max || output < data_min)
  {
    output = 0;
    println("Invalid data value: " + input + ". Sending 0 instead...");
  }
  return output;
}

/*
 * Send a command to the robot with the following protocol
 * 1. Send command begin message (constant)
 * 2. Send command data
 * 3. Send command end message (constant)
 * 
 * @requires serial is an open serial connection
 * @requires command is initialized
 */
void sendCommand(Serial serial, Command command)
{
  int[] data = command.getData();
  serial.write(command_begin);
  for(int i = 0; i < data.length; i++)
  {
    serial.write(sanitizeByte(data[i]));
  }
  serial.write(command_end);
}
