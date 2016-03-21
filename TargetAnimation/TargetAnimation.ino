#include <Servo.h> 
 
Servo target;
Servo target2;

int trigger = 13;
int idlepos = 25;
int downpos = 100;

void setup() 
{ 
  target.attach(9);  // attaches the servo on pin 9 to the servo object
  target.write(idlepos);
  
  target2.attach(10);  // attaches the servo on pin 9 to the servo object
  target2.write(idlepos);
  
  // trigger pin
  pinMode(trigger, INPUT);
  digitalWrite(trigger, LOW); // pull down
  
  // serial command interface
  Serial.begin(9600);
  
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH); // pull down
}

void hit(Servo& myservo)
{
    myservo.write(idlepos + 30);
    delay(50);
    myservo.write(idlepos);
}

void up(Servo& servo)
{
  for(int i = servo.read(); i >= idlepos; i--)
  {
    servo.write(i);
    delay(2);
  }
}

void down(Servo& servo)
{
  for(int i = servo.read(); i <= downpos; i++)
  {
    servo.write(i);
    delay(2);
  }
}


void loop() 
{
  if(digitalRead(trigger))
  {
    hit(target);
    down(target);
    up(target);
  }
  
  if(Serial.available())
  {
    byte input = Serial.read();
    switch(input)
    {
      case 'h':
        hit(target);
      break;
      
      case 'd':
        down(target);
      break;
      
      case 'u':
        up(target);
      break;
      
      case 'H':
        hit(target2);
      break;
      
      case 'D':
        down(target2);
      break;
      
      case 'U':
        up(target2);
      break;
      
      default:
      break;
    }
  }
}
