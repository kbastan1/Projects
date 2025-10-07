/*
  Automatic Candy Dispenser (Auger + Ultrasonic Trigger + Button + Buzzer)
  - NEMA 17 stepper driven by a step/dir driver (e.g., A4988/DRV8825/TMC).
  - Ultrasonic sensor (HC-SR04 style) detects a hand/container.
  - Button allows manual dispense while held.
  - Buzzer provides feedback beeps.
  
  NOTE:
  - this project was ran using a4988 stepers as noise is not a concern.
  - Tune step timing (delayMicroseconds) to match your driver, voltage, and motor torque needs.
  - stepsPerRevolution below is set to 100 (not typical 200); adjust this depending on auger pitch and microstepping.
*/

// ---------------- Pin Assignments ----------------
const int stepPin   = 2;   // STEP input on the stepper driver
const int dirPin    = 3;   // DIR input on the stepper driver
const int trigPin   = 9;   // ultrasonic trigger
const int echoPin   = 10;  // Ultrasonic echo
const int buttonPin = 5;   // Manual dispense button (HIGH when pressed if wired with pull-down)
const int buzzerPin = 11;  // Piezo buzzer pin (button is for debugging reasons, and a fallback if the sensor ever has a hardware failure)

// ---------------- State / Config -----------------
int  buttonState    = 0;    // raw read from buttonPin each loop
bool buttonIsActive = false;

// Steps per auger "revolution" (logical unit for one dispense action).
// Typical NEMA 17 full-step is 200 steps/rev, i set it to 100 because a 360 rotation of the auger dispenses too much.
const int stepsPerRevolution = 100;

// Thresholding logic for ultrasonic trigger:
// We require "Threshold" consecutive readings under the distance limit before dispensing.
// Helps filter noise and avoid false triggers.
// The higher the treshold, the less false triggers, but the longer you need to keep your hand in front of the sensor to activate it.
const int Threshold = 2;     // Number of consecutive close readings required
int currThreshold   = 0;     // Counter of consecutive close readings

void setup() {
  Serial.begin(9600);

  // Stepper driver control
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin,  OUTPUT);

  // Ultrasonic sensor 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Button input:
  // If you wire the button to ground and enable the internal pull-up, use:
  //   pinMode(buttonPin, INPUT_PULLUP);  // then pressed = LOW
  // For now we assume external pull-down to GND and 5V when pressed:
  pinMode(buttonPin, INPUT);

  // buzzer
  pinMode(buzzerPin, OUTPUT);
}

// Rotate auger forward to dispense candy.
// Increase the loop limit or adjust delays to change amount/speed.
void rotateStepper() {
  digitalWrite(dirPin, LOW); 
  for (int i = 0; i < stepsPerRevolution * 2; i++) { // "*2" = longer dispense
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(5000); // Step pulse width + speed control (bigger = slower)
    digitalWrite(stepPin, LOW);
    delayMicroseconds(5000); // Step interval (RPM control)
  }
}

// Retract auger slightly to prevent jams or dripping candy.
// Reverse direction relative to rotateStepper().
void retractStepper() {
  digitalWrite(dirPin, HIGH); // Opposite of rotateStepper()
  for (int i = 0; i < stepsPerRevolution; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(5000);
  }
}

// Simple blocking beep helper. it just beeps twice, thats it.
// btone: frequency in Hz; btone: lower btone means faster beeps.
void beep(int btone, int bDelay) {
  tone(buzzerPin, btone);
  delay(bDelay);
  noTone(buzzerPin);
  delay(bDelay);
}

// Measure distance (cm) from ultrasonic sensor.
// Returns ~0 if no echo received.
int getDistance() {
  // Trigger a 10 µs pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo pulse width (mus). Optionally add timeout: pulseIn(echoPin, HIGH, 30000);
  long duration = pulseIn(echoPin, HIGH);

  // Convert time to distance (cm). Speed of sound ~ 343 m/s.
  // distance (cm) = (duration (mu*s) * 0.034) / 2
  int distance = duration * 0.034 / 2;

  return distance;
}

void loop() {
  // ---------------- Manual Button Handling ----------------

  // The button is not intended to be used, but is a fallback in case of ultrasonic sensor failure.
  // also useful for debugging.
  
  // Reads HIGH when pressed with pull-down wiring.
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    // Manual dispense while button is held
    rotateStepper();
    buttonIsActive = true;
  } else {
    buttonIsActive = false;
  }

  // ---------------- Ultrasonic Reading ----------------
  Serial.print("Value: ");
  int distance = getDistance();
  Serial.println(distance);

  // require multiple consecutive "close" readings before acting. (filters noise)
  // Here, "close" means < 30 cm (adjust to your installation).
  if (distance < 30) {
    currThreshold = currThreshold + 1;
    Serial.println("upping one");
  } else {
    currThreshold = 0; // Reset when object moves away
    Serial.println("Cancelling");
  }

  // ----------------- Auto Dispense Logic ----------------
  // Only auto-dispense when threshold reached, and button is NOT currently active.
  if (currThreshold == Threshold && !buttonIsActive) {
    Serial.println("RAN");

    // Feedback beep before dispensing
    beep(1000, 500);

    // Dispense cycle: rotate to dispense, then retract once
    for (int i = 0; i < 1; i++) {
      rotateStepper();
      delay(10);
      retractStepper();
    }

    // Post-dispense pause + double beep confirmation
    delay(3000);
    beep(3000, 80);
    beep(3000, 80);
  }

  // Safety/UX: if someone leaves a bucket/hand blocking the sensor too long,
  // warn with a long beep (prevents repeated triggers).
  // this is useful to prevent greedy users who keep their hand in front of the sensor to get unlimited candy
  if (currThreshold > 3) { // "3" here means >3 * loop cycles of being close
    beep(500, 1000);
    Serial.println("delaying, object blocking sensor"); // debugging message, user will not see this.
  }

  // Overall loop pacing. Keep small to maintain responsiveness,
  delay(100);
}
