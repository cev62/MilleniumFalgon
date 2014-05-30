import processing.serial.*;

final int handshake_response_bluetooth = 240;
final int handshake_response_wired = 208;
final int handshake_begin = 224;
final int handshake_end = 192;
final int command_begin = 224;
final int command_end = 192;
final int data_max = 127;
final int data_min = 0;

int[] password = {1, 2, 3, 4};
final int handshake_response_timeout = 1000; // milliseconds

Serial serial;
Command command;
boolean up = false, down = false, left = false, right = false, key_change = false;
int timer;

void setup()
{
 println(Serial.list());
 serial = new Serial(this, "COM15", 9600);
 
 int response = handshake(serial, password); // Password must be 4 digits right now
 if(response == handshake_response_bluetooth)
 {
   println("Successfull handshake via bluetooth connection.");
 }
 else if(response == handshake_response_wired)
 {
   println("Successfull handshake via wired connection.");
 }
 else
 {
   println("Unsuccessfull handshake.");
 }
 println("Response: " + response);
 
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
    sendCommand(serial, command);
    println("(" + up + ", " + down + ", " + left + ", " + right + ")");
    key_change = false;
  }
  if(millis() - timer > 100)
  {
    sendCommand(serial, command);
    timer = millis();
  }
  if(serial.available() > 0)
  {
    while(serial.available() > 0)
    {
      print((char)serial.read());
    }
    println();
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
  boolean up, down, left, right;
  Command()
  {
    up = false;
    down = false;
    left = false;
    right = false;
  }
  
  /*
   * Exports the command to a series of bytes for transmission
   */
  int getData()
  {
    int data = 0;
    // Puts the booleans into the 4 least significant digits of a single byte
    data = (right ? 1 : 0) + (left ? 2 : 0) + (down ? 4 : 0) + (up ? 8 : 0);
    println(data);
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
 * Perform a handshake over a serial connection
 * 1. Send handshake begin message (constant)
 * 2. Send password
 * 3. Send handshake end message (constant)
 * 4. Wait for response intil timeout
 * 5. If response times out, return 0
 * 6. If response is received, and it is the handshake response bluetooth (constant)
 *    or the handshake_response_wired (constant) then return that value
 *
 * @requires serial is an open serial connection 
 */
int handshake(Serial serial, int[] password)
{
  // Flush the serial buffer
  while(serial.available() > 0)
  {
    serial.read();
  }
  println("Start");
  serial.write(handshake_begin);
  /*for(int i = 0; i < password.length; i++)
  {
    serial.write(sanitizeByte(password[i]));
  }*/
  serial.write(handshake_end);
  
  int timeout = millis();
  while(millis() - timeout < handshake_response_timeout)
  {
    // Wait for a response
    println("Waiting");
    
    if(serial.available() > 0)
    {
      int response = serial.read();
      println("Actual: " + response);
      return response;
      /*if(response == handshake_response_bluetooth || response == handshake_response_wired)
      {
        return response;
      }
      else
      {
        return 0;
      }*/
    }
  }
  return -1;
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
  int data = sanitizeByte(command.getData());
  serial.write(command_begin);
  serial.write(data);
  serial.write(command_end);
}
