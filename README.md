# Halloween Candy Dispenser (OCT 2024)
Automatic Candy Dispenser (Ultrasonic + Stepper + Buzzer)

An Arduino-based automatic candy dispenser that uses an ultrasonic sensor to detect when a hand or container is placed near the outlet and automatically rotates an auger (driven by a NEMA 17 stepper motor) to dispense candy. The system also supports manual dispensing via a push button and includes buzzer feedback for user interaction and safety.

Features

Hands-free dispensing using a cheap and widely available ultrasonic distance sensor (HC-SR04)

Manual override button to dispense candy on demand (incase of hardware failure)

Stepper-driven auger ensures consistent portion control

Buzzer feedback for start, completion, and error/warning signals

Anti-block logic to prevent repeated triggers if an object stays too long under the sensor

Simple threshold filtering for stable distance detection

Hardware Requirements

Arduino Uno (or compatible board)

NEMA 17 stepper motor + driver (A4988 / DRV8825 / TMC2208, etc.)

HC-SR04 ultrasonic sensor

Push button

Piezo buzzer

Power supply (5–12V depending on driver and motor)

How It Works:

The ultrasonic sensor continuously measures distance.

When an object (like a hand waving) is detected below a threshold for a short duration, the stepper motor activates the auger to release candy.

The buzzer plays feedback tones during and after dispensing.

A manual button allows the user to dispense candy directly at any time.

If the sensor remains triggered too long, the system warns the user and pauses operation.

Customization:

Adjust stepsPerRevolution and loop multipliers to fine-tune the amount dispensed per cycle.

Modify the ultrasonic threshold distance (if (distance < 30)) for your setup.

Change beep frequencies or durations in beep() for different sound patterns.

Future Improvements:

Add a stronger stepper motor to assist with stubborn candy jams.
