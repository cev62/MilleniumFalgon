import processing.serial.*;

Serial serial;
boolean up = false, down = false, left = false, right = false, key_change = false;

void setup()
{
 println(Serial.list());
 serial = new Serial(this, Serial.list()[0], 9600);
 serial.write(0);
}

void draw()
{
  if(key_change)
  {
    float left_power = (up ? 1.0 : 0) + (down ? -1.0 : 0) + (left ? -1.0: 0) + (right ? 1.0 : 0);
    float right_power = (up ? 1.0 : 0) + (down ? -1.0 : 0) + (left ? 1.0: 0) + (right ? -1.0 : 0);
    left_power = max(-1.0, min(1.0, left_power));
    right_power = max(-1.0, min(1.0, right_power));
    setDrive(left_power, right_power);
    println("(" + left_power + ", " + right_power + ")");
    key_change = false;
  }
  if(serial.available() > 0)
  {
    println(serial.read());
  }
}

void keyPressed()
{
  if(key == CODED)
  {
    if(keyCode == UP && !up)
    {
      up = true;
      key_change = true;
    }
    if(keyCode == DOWN && !down)
    {
      down = true;
      key_change = true;
    }
    if(keyCode == LEFT && !left)
    {
      left = true;
      key_change = true;;
    }
    if(keyCode == RIGHT && !right)
    {
      right = true;
      key_change = true;
    }
  }
}

void keyReleased()
{
  key_change = true;
  if(key == CODED)
  {
    if(keyCode == UP)
    {
      up = false;
    }
    if(keyCode == DOWN)
    {
      down = false;
    }
    if(keyCode == LEFT)
    {
      left = false;
    }
    if(keyCode == RIGHT)
    {
      right = false;
    }
  }
}

void setDrive(float left, float right)
{
  serial.write((int)(left * 255*1.0) + "," + (int)(right * 255*1.0) + ";");
}
