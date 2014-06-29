#include <SoftwareSerial.h>

//SoftwareSerial softSerial(9, 10); // RX, TX
int i = 0;
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(13, OUTPUT);
}

void loop()
{
  //Serial.println("Hi");
  /*if(Serial.available())
  {
    softSerial.print(Serial.read());
  }*/
  //softSerial.println("leonardo");
  //Serial.println("leonardo");
  //delay(100);
  
  
  
  /*if(softSerial.available())
  {
    Serial.print("soft:");
    char r = (char)softSerial.read();
    Serial.println(r);
    if(r == 'h'){
      digitalWrite(13, HIGH);
      i = 1;
    }
    if(r == 'l'){
      digitalWrite(13, LOW);
      i = 0;
    }
  }*/
  
  //softSerial.print("hi");
  
  Serial1.print('h');
  digitalWrite(13, HIGH);
  delay(100);
  Serial1.print('l');
  digitalWrite(13, LOW);
  delay(100);
  
}
