/* teensyBlink.ino 
 Christopher Datsikas
 Date Created: 03-20-2016
 Date Modified: 03-20-2016
 Status: Works!
 Adapted from Teensy documentation
 Run this code to verify connection with Teensy 3.2
 Special Teensy window should appear when code is uploaded
 In my case:
 Board is Teensy 3.2 / 3.1 on
 Serial /dev/tty.usbmodem1548301
 */

/* LED Blink, Teensyduino Tutorial #1
   http://www.pjrc.com/teensy/tutorial.html
   This example code is in the public domain.
*/

// Teensy 3.2 has the LED on pin 13
// No other connections necessary

const int ledPin = 13;
const int delayPeriod = 1000;  // change to verify upload

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);  // set the LED on
  delay(delayPeriod);          // wait for a delayPeriod
  digitalWrite(ledPin, LOW);   // set the LED off
  delay(delayPeriod);          // wait for a second
}

