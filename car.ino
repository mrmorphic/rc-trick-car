/*
  Program to control an RC car. The driver can control it normally using a transmitter, but can invoke
  a pre-programmed trick.

  Authors: Mark Stephens, Ben Ellis
  Copyright (C) Mark Stephens 2013

  Licence: BSD


  The hardware set-up is:
  - an old RC car, complete with 2 channel transmitter, receiver on the car, connected to 2 servos.
  - one servo drives the steering from left to right.
  - one servo drives the rear wheels motor, in a continuum from full forward to full reverse.
  - the control wire from the receiver to the steering servo is cut. The receiver end is connected
    to an Arduino on digital pin 4 (RCVR_IN_LR_PIN below). The servo end is connected to the Arduino
    on digital pin 2 (STEERING_SERVO_PIN)
  - the control wire from the receiver to the speed servo is cut. The receiver end is connected to an
    Arduino on digital pin 5 (RCVR_IN_SPEED_PIN). The servo end is connected to the Arduino on
    digital pin 3 (SPEED_SERVO_PIN)
  In this way the Arduino sits between the receiver and the servos, so the Arduino can transform the
  values from the transmitter in real time.

  This program does the following
  - Values read for both servos are read and then sent to the servos in real time, without any
    change in the ultimate angles sent to the servos. Thus the user still controls the car with the
    transmitter.
  - If the speed control is pushed to maximum reverse, this triggers the car to perform a trick,
    in this case a 3 point turn where the car ends up in the position but facing the opposite direction.

  Many of the constants were determined experimentally (servo limits, receiver values for example). These
  would probably need some adjustment on different hardware.

  If you have more channels, you could use additional channels for more tricks.
*/

// Pins:
// D2 - servo for front steering.
// D3 - servo for main motor speed
// D4 - servo signal from receiver for front steering
// D5 - servo signal from received for main motor speed


#include <Servo.h> 

// Constants for servo pins
#define STEERING_SERVO_PIN  2
#define SPEED_SERVO_PIN     3
#define RCVR_IN_LR_PIN      4
#define RCVR_IN_SPEED_PIN   5

// Constants for steering angles
#define STEERING_ANGLE_HARD_LEFT   55
#define STEERING_ANGLE_STRAIGHT    78
#define STEERING_ANGLE_HARD_RIGHT  100

// Note that forward is lower values than reverse
#define SPEED_ANGLE_MAX_FORWARD   60
#define SPEED_ANGLE_STATIONARY    90
#define SPEED_ANGLE_MAX_REVERSE  120

// Constants for left/right values (microseconds as unsigned longs) from receiver
#define RCVR_IN_LR_MIN    1069
#define RCVR_IN_LR_MAX    1746

// Constants for up/down values (microseconds as unsigned longs) from receiver
#define RCVR_IN_SPEED_FAST       1047
#define RCVR_IN_SPEED_REVERSE    1746

// Return true if we should trigger the trick. In this case, if full reverse is detected.
#define TURN_TRICK_THRESHOLD(p) (p > 1740)

// Servo objects for the two connected servo motors.
Servo frontSteering; 
Servo thebigmotor;

// prototypes
void set_speed(int s);
void set_angle(int a);
void three_point_turn();

void setup() 
{
  // Set up the servos
  frontSteering.attach(STEERING_SERVO_PIN);
  thebigmotor.attach(SPEED_SERVO_PIN);

  // Set up the receiver input pns
  pinMode(RCVR_IN_LR_PIN, INPUT);
  pinMode(RCVR_IN_SPEED_PIN, INPUT);
  
  set_speed( SPEED_ANGLE_STATIONARY );
}
  
void loop() 
{
  // read from receiver
  unsigned long leftRightWidth = pulseIn(RCVR_IN_LR_PIN, HIGH);
  unsigned long speedWidth = pulseIn(RCVR_IN_SPEED_PIN, HIGH);
 
  if (TURN_TRICK_THRESHOLD((unsigned int) speedWidth)) {
    three_point_turn();
  }
  else {
    // translate receiver inputs into speed and direction outputs
    int angle = map((unsigned int) leftRightWidth, RCVR_IN_LR_MIN, RCVR_IN_LR_MAX, STEERING_ANGLE_HARD_LEFT, STEERING_ANGLE_HARD_RIGHT);
    int spd = map(speedWidth, RCVR_IN_SPEED_FAST, RCVR_IN_SPEED_REVERSE, SPEED_ANGLE_MAX_FORWARD, SPEED_ANGLE_MAX_REVERSE);
    set_angle(angle);
    set_speed(spd);
  }
}

// Perform a 3-point turn. Timings were determined experimentally. Furniture was dented. Cats were frightened.
void three_point_turn() {
  set_angle(STEERING_ANGLE_STRAIGHT);
  set_speed(SPEED_ANGLE_MAX_FORWARD );
  delay(1250);
  set_angle(STEERING_ANGLE_HARD_LEFT);
  delay(1000);
  set_speed(SPEED_ANGLE_MAX_REVERSE);
  set_angle(STEERING_ANGLE_HARD_RIGHT);
  delay(1650);
  set_angle(STEERING_ANGLE_STRAIGHT);
  set_speed(SPEED_ANGLE_MAX_FORWARD);  
  delay(1250);
}

// Set the speed. This just writes the value to the servo, so the value should be in the supported range.
void set_speed(int s) {
  thebigmotor.write(s);
  delay(15);
}

// Set the angle. Same as for set_speed()
void set_angle(int a) {
  frontSteering.write(a); 
  delay(15); 
}


