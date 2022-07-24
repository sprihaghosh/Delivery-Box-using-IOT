#include <Keypad.h>
#include <Servo.h>

#define ROW_NUM     4  // four rows
#define COLUMN_NUM  4  // four columns
int position = 0;

Servo servo1;
static const int servoPin = 13;
int sensorPin = 7;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {19, 18, 5, 17};    // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
byte pin_column[COLUMN_NUM] = {16, 4, 0, 2}; // GIOP16, GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup() 
{
  Serial.begin(115200);
  pinMode(sensorPin,INPUT);
  servo1.attach(servoPin);
  openbox();
}

void loop() {

  String password = "";
  int val = digitalRead(sensorPin);
  if (val==0)
  {openbox();
  }
  else if (val==1)
  delay(10000);
  {closebox();
  //password = String(random(1000,10000));
  password = "4321";
  }

  char key = keypad.waitForKey();
  
  if(password != ""){

    int pos = 0;
    int t = 0;
    if(key=='*' || key=='#'){
    pos = 0;
    closebox();
    }

    boolean p;
    while(p = true){
      int c;
      for(c=0;c<4;c++){

         int correctpw = 0;
         for(pos=0; pos<5; pos++){
           if (key==password[pos]){
           correctpw++;}
         }
         
         if(correctpw == 4){ //if password entered is correct
          openbox();
          p = false;
          break;}

         else{
           
           if(c==3){ //for every 3 attempts made, time gets delayed by 10 minutes 
             t = t+600000;
             delay(t);
             p = false;
           }
           else{  //1st and 2nd attempt, gets delayed by 30 seconds
              delay(30000);
              c+=1;           
           }
         }
      }
    }
  }
}

void closebox()
{
  int pos=0;
  for(pos = 0; pos < 90; pos += 1)  // goes from 0 degrees to 90 degrees 
  {                                  // in steps of 1 degree 
    servo1.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  delay(500);
}

void openbox()
{
  int pos = 0;
  for(pos = 90; pos>=1; pos-=1)     // goes from 90 degrees to 0 degrees 
  {                                
    servo1.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
    delay(500);
}    
