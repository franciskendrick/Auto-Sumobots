/*
  Microcontroller: Arduino UNO R3
  Motor Driver: L298N
  DC Motor: TT Motor
  No. of DC Motors: 4
  Ideal Voltage: 8v

  Line Sensor: 1-Channel TCRT5000
  No. of Line Sensor: 1
  Position of Line Sensor/s: Front-center

  Distance Sensor: HC-SR04 Ultrasonic 
  No. of Distance Sensor: 2
  Position of Distance Sensor/s: Both in front
*/

// Initialize Motor pins
// Right motor
const int PWM_right = 10;
const int motor_RB = 8;
const int motor_RF = 9;

// Left motor
const int PWM_left = 5;
const int motor_LB = 7;
const int motor_LF = 6;

// Speed constants
const int forward_speed = 110;
const int attack_speed = 255;
const int sharpturn_speed = 255;
const int pivotturn_speed = 120;

// Initialize ultrasonic sensor pins
// Right ultrasonic
const int R_trigger = 3;
const int R_echo = 4;
long R_duration;
long R_distance;

// Left ultrasonic
const int L_trigger = 11;
const int L_echo = 12;
long L_duration;
long L_distance;

// Time variables for edge detection
long lasttime = millis();
long currenttime;
const int period = 537;  // 375ms + 255PWM = 90 degrees

// Edge detection variables
bool on_edge = false;
int sightrange = 45;  // cm

void setup() {
  Serial.begin(9600);

  // Motor pin setup
  pinMode(motor_RF, OUTPUT);
  pinMode(motor_RB, OUTPUT);
  pinMode(motor_LF, OUTPUT);
  pinMode(motor_LB, OUTPUT);

  // Ultrasonic sensor pin setup
  pinMode(R_trigger, OUTPUT);
  pinMode(R_echo, INPUT);
  pinMode(L_trigger, OUTPUT);
  pinMode(L_echo, INPUT);
}

void loop() {
  // Read line sensor value
  int linesensor = digitalRead(13);

  // Get distance from ultrasonic sensors
  R_distance = getdistance(R_trigger, R_echo);
  L_distance = getdistance(L_trigger, L_echo);

  // Edge detection and handling
  currenttime = millis();
  if (on_edge) {
    if (currenttime - lasttime >= period) {
      on_edge = false;
    } else {
      sharpright(); // Continue turning sharply while on edge
      on_edge = true;
    }
  }

  // Print sensor values for debugging
  Serial.print("R-Distance: ");
  Serial.print(R_distance);
  Serial.print(" L-Distance: ");
  Serial.print(L_distance);
  Serial.print(" Line: ");
  Serial.print(linesensor);
  Serial.print(" Delta: ");
  Serial.print(currenttime - lasttime);
  Serial.print(" Edge: ");
  Serial.println(on_edge);

  // White = 1; Black = 0
  if (linesensor == 1) {  // robot in arena (sees white)

    // Check if an enemy is within sight range
    if (L_distance <= sightrange || R_distance <= sightrange) {
      if (abs(L_distance - R_distance) <= 5) {  // enemy in the center
        attack();
      } else if (R_distance > L_distance) {  // enemy in the left
        attack_left();
      } else if (L_distance > R_distance) {  // enemy in the right
        attack_right();
      }
    }
    else {
      forward();  // Move forward if no enemy detected
    }

  } else {  // robot on edge (sees black)
    on_edge = true;
    lasttime = millis();  // Record the time when the edge is detected
  }
}

// Function to get distance from ultrasonic sensor
long getdistance(int trigger, int echo) {
  long duration;
  long distance;

  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.0343 / 2;

  return distance;
}

// Motor control functions
void stop() {
  digitalWrite(motor_RF, LOW);
  digitalWrite(motor_RB, LOW);
  digitalWrite(motor_LF, LOW);
  digitalWrite(motor_LB, LOW);

  analogWrite(PWM_right, 0);
  analogWrite(PWM_left, 0);
}

void forward() {
  digitalWrite(motor_RF, HIGH);
  digitalWrite(motor_RB, LOW);
  digitalWrite(motor_LF, HIGH);
  digitalWrite(motor_LB, LOW);

  analogWrite(PWM_right, forward_speed);
  analogWrite(PWM_left, forward_speed);
}

void attack() {
  digitalWrite(motor_RF, HIGH);
  digitalWrite(motor_RB, LOW);
  digitalWrite(motor_LF, HIGH);
  digitalWrite(motor_LB, LOW);

  analogWrite(PWM_right, attack_speed);
  analogWrite(PWM_left, attack_speed);
}

void attack_left() {
  digitalWrite(motor_RF, HIGH);
  digitalWrite(motor_RB, LOW);
  digitalWrite(motor_LF, HIGH);
  digitalWrite(motor_LB, LOW);

  analogWrite(PWM_right, attack_speed);
  analogWrite(PWM_left, pivotturn_speed);
}

void attack_right() {
  digitalWrite(motor_RF, HIGH);
  digitalWrite(motor_RB, LOW);
  digitalWrite(motor_LF, HIGH);
  digitalWrite(motor_LB, LOW);

  analogWrite(PWM_right, pivotturn_speed);
  analogWrite(PWM_left, attack_speed);
}

void sharpleft() {
  digitalWrite(motor_RF, HIGH);
  digitalWrite(motor_RB, LOW);
  digitalWrite(motor_LF, LOW);
  digitalWrite(motor_LB, HIGH);

  analogWrite(PWM_right, sharpturn_speed);
  analogWrite(PWM_left, sharpturn_speed);
}

void sharpright() {
  digitalWrite(motor_RF, LOW);
  digitalWrite(motor_RB, HIGH);
  digitalWrite(motor_LF, HIGH);
  digitalWrite(motor_LB, LOW);

  analogWrite(PWM_right, sharpturn_speed);
  analogWrite(PWM_left, sharpturn_speed);
}