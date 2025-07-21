// Pin Assignments
const int TRIG       = 12;  // Ultrasonic trigger
const int ECHO       = 13;  // Ultrasonic echo

// Left motor (L298N)
const int IN1        = 10;
const int IN2        = 11;
const int ENA        = 9;   // PWM

// Right motor (L298N)
const int IN3        = 8;
const int IN4        = 7;
const int ENB        = 6;   // PWM

// LEDs
const int RED_LED    = 4;
const int YELLOW_LED = 2;
const int GREEN_LED  = 3;

// Replay button
const int BUTTON     = 5;  // D5

// Path memory
char pathLog[100];
int idx = 0;

long duration;
int distance;

void setup() {
  // Motor outputs
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);

  // Ultrasonic
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // LEDs
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button with pull-up
  pinMode(BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  // If replay button pressed, run replay routine
  if (digitalRead(BUTTON) == LOW) {
    replayPath();
    idx = 0;  // clear log
  }

  distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 30) {
    indicate(GREEN_LED);
    drive(255, 255, false, false);
    record('F');
  }
  else if (distance > 15) {
    indicate(YELLOW_LED);
    drive(128, 128, true, false);
    record('R');
  }
  else {
    indicate(RED_LED);
    drive(128, 128, false, true);
    record('L');
  }

  delay(100);
}

int getDistance() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
  return duration * 0.034 / 2;
}

void drive(int lPWM, int rPWM, bool leftRev, bool rightRev) {
  digitalWrite(IN1,  leftRev  ? HIGH : LOW);
  digitalWrite(IN2,  leftRev  ? LOW  : HIGH);
  analogWrite(ENA,  constrain(lPWM, 0, 255));

  digitalWrite(IN3,  rightRev ? HIGH : LOW);
  digitalWrite(IN4,  rightRev ? LOW  : HIGH);
  analogWrite(ENB,  constrain(rPWM, 0, 255));
}

void indicate(int led) {
  digitalWrite(RED_LED,    led == RED_LED    );
  digitalWrite(YELLOW_LED, led == YELLOW_LED );
  digitalWrite(GREEN_LED,  led == GREEN_LED  );
}

void record(char c) {
  if (idx < (int)sizeof(pathLog) - 1) {
    pathLog[idx++] = c;
  }
}

void replayPath() {
  drive(0, 0, false, false);
  delay(200);

  Serial.println("Replaying path:");
  for (int i = idx - 1; i >= 0; --i) {
    char m = pathLog[i];
    Serial.print("  Step "); Serial.print(i); Serial.print(": "); Serial.println(m);
    if (m == 'F')       drive(255, 255, false, false);
    else if (m == 'R')  drive(128, 128, false, true);
    else if (m == 'L')  drive(128, 128, true, false);
    delay(300);
  }

  drive(0, 0, false, false);
  Serial.println("Replay complete.");
}
