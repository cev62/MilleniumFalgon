#include <SoftwareSerial.h>

SoftwareSerial blueSerial(9, 10); // RX, TX

void setup()
{
  Serial.begin(9600);
  while(!Serial){}
  Serial.write("Type AT commands\n");
  blueSerial.begin(9600);
}

void loop()
{
  if(Serial.available())
  {
    blueSerial.write(Serial.read());
  }
  if(blueSerial.available())
  {
    Serial.write(blueSerial.read());
  }
}
