#include <Arduino.h>
#include <PPMReader.h>
#include <DifferentialSteering.h>
#include <Servo.h>

int maxVal = 1900;
int minVal = 1050;

int throttle = 0; //channel 2
int direction = 0; //channel 1
int weapon = 0; //channel 5
int hat = 0; //channel 3

int leftMotor = 0;
int rightMotor = 0;

int fPivYLimit = 32;
DifferentialSteering DiffSteer;


int motor1pin1 = 12; //connect to pin 1 on h-bridge 1
int motor1pin2 = 11; //connect to pin 2 on h-bridge 1
int motor1pwm  = 5;  //connect to enable pin on h-bridge 1

int motor2pin1 = 10; //connect to pin 1 on h-bridge 2
int motor2pin2 = 9;  //connect to pin 2 on h-bridge 2
int motor2pwm  = 6;  //connect to enable pin on h-bridge 2

int weaponRelayPin = 13;

int p = 0;

int leftMotorThrottle = 0;
int rightMotorThrottle = 0;

// Initialize a PPMReader on digital pin 3 with 6 expected channels.
byte interruptPin = 3;
byte channelAmount = 6;
PPMReader ppm(interruptPin, channelAmount);

Servo servo;

void setup() {
    Serial.begin(9600);
    DiffSteer.begin(fPivYLimit);


    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);

    pinMode(weaponRelayPin, OUTPUT);

    analogWrite(motor1pwm, 255);
    analogWrite(motor2pwm, 255);

    servo.attach(A5);

}

void loop() {

    
    
    throttle = ppm.latestValidChannelValue(2, 0); //read channel 2
    direction = ppm.latestValidChannelValue(1, 0); //read channel 1
    weapon = ppm.latestValidChannelValue(5, 0); //read channel 5
    hat = ppm.latestValidChannelValue(3, 0); //read channel 3

    servo.write(map(hat, 1050, 1900, 0, 180));

    throttle = map(throttle, minVal, maxVal, -127, 127);
    direction = map(direction, minVal, maxVal, -127, 127);

    DiffSteer.computeMotors(direction, throttle);
    
    leftMotor = DiffSteer.computedLeftMotor();
    rightMotor = DiffSteer.computedRightMotor();

    //Comment out print statements when you are done testing, just to make stuff run a bit faster

    Serial.print(" L: ");

    if(leftMotor > 10){
      Serial.print("Forward ");
      digitalWrite(motor1pin1, HIGH);
      digitalWrite(motor1pin2, LOW);
      leftMotorThrottle = map(leftMotor, 0, 127, 0, 255);
    }

    else if(leftMotor < -10){
      Serial.print("Reverse ");
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, HIGH);
      leftMotorThrottle = map(leftMotor, -127, 0, 255, 0);
    }

    else {
      Serial.print("Stop ");
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, LOW);
      leftMotorThrottle = 0;
    }

    Serial.print(" R: ");

    if(rightMotor > 10){
      Serial.print("Forward ");
      digitalWrite(motor2pin1, HIGH);
      digitalWrite(motor2pin2, LOW);
      rightMotorThrottle = map(rightMotor, 0, 127, 0, 255);
    }

    else if(rightMotor < -10){
      Serial.print("Reverse ");
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, HIGH);
      rightMotorThrottle = map(rightMotor, -127, 0, 255, 0);
    }

    else {
      Serial.print("Stop ");
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, LOW);
      rightMotorThrottle = 0;
    }

    Serial.print(" W: ");

    if(weapon > 1500){
      digitalWrite(weaponRelayPin, HIGH);
      Serial.print("ON");
    }

    else{
      digitalWrite(weaponRelayPin, LOW);
      Serial.print("OFF");
    }

    Serial.println();    

    analogWrite(motor1pwm, leftMotorThrottle);
    analogWrite(motor2pwm, rightMotorThrottle);

    delay(40);
}