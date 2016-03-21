/* opticalTransmitterTest.ino 
 Christopher Datsikas
 Adapted from Teensy documentation, BlinkWithoutDelay.ino Arduino Example, Geek Group
 Date Created: 03-20-2016
 Date Modified: 03-20-2016
 Status: Works!
 Run this code to verify operation of optical transmitter
 IF-E96E should blink red
 AND On-Board LED will blink to show that Board has power and is working
 
 EQUIPMENT
 Teensy 3.2
 IF-E96E 
 330 Ohm Resistor
 ULN2803A Darlington Transistor Array
 
 CONNECTIONS
 Teensy D3           -> ULN2803A Pin 1B/1
 ULN2803A Pin 1C/18  -> 330 Ohm Resistor -> IF-E96E Cathode (-) (short)
 IF-E96E Anode (+) (long) <-> VIN_3V3
 */
 
#define ledPin   13             // On-board Chip LED
#define outPin   3              // Sinks IF-E96E

// LED variables
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long ledInterval = 1000;           // interval at which to blink (milliseconds)

// Tx variables
float freq = 10;                // Change the frequency of the tone here (in Hz)
float period;
float duty = .50;               //Change the duty cycle of the tone here
int txState = LOW;
unsigned long ontime, offtime;
unsigned long currentMicros = 0;    // will overflow after 70 minutes
unsigned long previousMicros = 0;
unsigned long txInterval = offtime;

void setup() {
  // set the digital pin as output;
  pinMode(ledPin, OUTPUT);
  pinMode(outPin, OUTPUT);
  period = 1/freq;                      // period is inverse of frequency (math)
  ontime = 1000000*period*duty;         // in us
  offtime = 1000000*period*(1.0-duty);  // in us
  
  delay(100);
  // for debugging
  Serial.begin(9600);
  while (!Serial);         // while the serial stream is not open, do nothing
  Serial.println(period);
  Serial.println(ontime);
  Serial.println(offtime);
}

void loop()
{
  
  // check to see if it's time to blink the LED; that is, if the 
  // difference between the current time and last time you blinked 
  // the LED is bigger than the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > ledInterval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  
  
  currentMicros = micros();
  if (currentMicros - previousMicros > txInterval) {
    previousMicros = currentMicros;
    
    if (txState == LOW) {
      txState = HIGH;
      txInterval = ontime;
    }
    else if (txState == HIGH) {
      txState = LOW;
      txInterval = offtime;
    }
    
    digitalWrite(outPin, txState);
  }
  
}
