/* LED Blink, Teensyduino Tutorial #1
   http://www.pjrc.com/teensy/tutorial.html
 
   This example code is in the public domain.
*/

// Teensy 2.0 has the LED on pin 11
// Teensy++ 2.0 has the LED on pin 6
// Teensy 3.0 has the LED on pin 13
#define OutPin   3
#define ledPin   13
float freq = 20;          //Change the frequency of the tone here
float period;
float duty = .50;          //Change the duty cycle of the tone here
unsigned long ontime, offtime, start;

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(OutPin, OUTPUT);
  period=1/freq;
  ontime=1000000*period*duty;         // in us
  offtime=1000000*period*(1.0-duty);  // in us
}

void loop()
{
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to 
  // blink the LED.
  
//  unsigned long currentMillis = millis();
//  if(currentMillis - previousMillis > interval) {
//    // save the last time you blinked the LED 
//    previousMillis = currentMillis;   
//
//    // if the LED is off turn it on and vice-versa:
//    if (ledState == LOW)
//      ledState = HIGH;
//    else
//      ledState = LOW;
//
//    // set the LED with the ledState of the variable:
//    digitalWrite(ledPin, ledState);
//  }
  
  start=micros();
  digitalWrite(OutPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  while (micros()<start+ontime);
  digitalWrite(OutPin, LOW);    // turn the LED off by making the voltage LOW
  while (micros()<start+ontime+offtime);
  
  
}
