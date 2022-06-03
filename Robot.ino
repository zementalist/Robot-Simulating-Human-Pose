#include <Servo.h>
Servo leftArm;
Servo head;
Servo rightArm;

int leftArmDownAngle = 50;
int leftArmStraightAngle = 130;
int leftArmUpAngle = 180;

int rightArmDownAngle = 140;
int rightArmStraightAngle = 60;
int rightArmUpAngle = 0;

int headLeft = 20;
int headStraightAngle = 100;
int headRight = 180;

// default signals
int signals[3] = {leftArmDownAngle, headStraightAngle, rightArmDownAngle};

// available states (down, straight, up) for each part
int leftArmStates[3] = {leftArmDownAngle,leftArmStraightAngle,leftArmUpAngle};
int rightArmStates[3] = {rightArmDownAngle,rightArmStraightAngle,rightArmUpAngle};
int headStates[3] = {headLeft, headStraightAngle, headRight};

// incoming signal as string
String value;

// Function to ~ convert "1,20,300" to [1, 20, 300]
void setSignal(String message, int (&arr)[3]) {
  String part1 = message;
  String part2 = message;
  int commaIndex;
  for(int i = 0; i < 3; i++) {
     commaIndex = part1.indexOf(',');
     part1 = message.substring(0, commaIndex);
    message = message.substring(commaIndex+1);
    arr[i] = part1.toInt();
  }
  arr[2] = message.toInt();
}

// factor represents sensitivity for the decision of moving any part
int factor = 800;

// The following map functions, maps the signal into an angle
// (i.e. 0 = head->left  or arm->down, 1 = head->straight, etc..)

int mapHead(int x) {
  return map(x, 0.4*800, 0.6*300, headLeft, headRight);
}
int mapLeftArm(int x) {
  return map(x, 0, 0.6*factor, leftArmDownAngle, leftArmUpAngle);
}

int mapRightArm(int x) {
  return map(x, 0, 0.6*factor, rightArmDownAngle, rightArmUpAngle);
}


// The following 3 functions are used to move robot parts
// based on the received signal
// Implementation can be improved by generalizing a function
// to do the job of those 3 functions

void moveHead(int x) {
  if(x == 0) {
    head.write(headLeft);
  }
  else if(x == 1){
    head.write(headStraightAngle);
  }
  else if(x == 2){
    head.write(headRight);
  }
  
}


void moveLeftArm(int x) {
  if(x == 0) {
    leftArm.write(leftArmDownAngle);
  }
  else if(x == 1){
    leftArm.write(leftArmStraightAngle);
  }
  else if(x == 2){
    leftArm.write(leftArmUpAngle);
  }
  
}

void moveRightArm(int x) {
  if(x == 0) {
    rightArm.write(rightArmDownAngle);
  }
  else if(x == 1){
    rightArm.write(rightArmStraightAngle);
  }
  else if(x == 2){
    rightArm.write(rightArmUpAngle);
  }
  
}

void setup() {

  // Set pins
  leftArm.attach(8);
  head.attach(9);
  rightArm.attach(10);

  // Initialize robot pose (arms down, head straight)
  leftArm.write(leftArmDownAngle);
  head.write(headStraightAngle);
  rightArm.write(rightArmDownAngle);

  Serial.begin(115200);
  Serial.setTimeout(1);
}

void loop() {
  if(Serial.available() > 0){
   value = Serial.readString();

  setSignal(value, signals);

  moveLeftArm(signals[1]);
  moveHead(signals[0]);
  moveRightArm(signals[2]);
  }
}
