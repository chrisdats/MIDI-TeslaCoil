/* opticalTransmitterTest.ino 
 Christopher Datsikas
 Adapted from Teensy documentation, BlinkWithoutDelay.ino Arduino Example, Geek Group
 Date Created: 03-20-2016
 Date Modified: 05-03-2016
 Status: Works!
 Run this code to verify operation of optical transmitter
 IF-E96E should blink red
 AND On-Board LED will blink to show when transmission is occuring
 In present version need to open Serial Monitor for code to run.
 
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
#define txPin   6              // Sinks IF-E96E


// Tx variables
//float freq = 1;
//float duty = 0.5;

float freq = 100;                // Change the frequency of the tone here (in Hz)
float period;
float duty = (1-.025);               //Change the duty cycle of the tone here
int txState = LOW;
unsigned long ontime, offtime;
unsigned long currentMicros = 0;    // will overflow after 70 minutes
unsigned long previousMicros = 0;
unsigned long txInterval = offtime;

void setup() {
  // set the digital pin as output;
  pinMode(ledPin, OUTPUT);
  pinMode(txPin, OUTPUT);
  period = 1/freq;                      // period is inverse of frequency (math)
  ontime = 1000000*period*duty;         // in us
  offtime = 1000000*period*(1.0-duty);  // in us
  
  // for debugging. need to open Serial Monitor for program to run
  Serial.begin(9600);
  //while (!Serial);         // while the serial stream is not open, do nothing
  Serial.println(period);
  Serial.println(ontime);
  Serial.println(offtime);
  Serial.println("Setup Complete");

  digitalWrite(txPin, HIGH);
}

void loop()
{
  
  // same procedure as above but modification to allow for
  // different ontime and offtime
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
    
    digitalWrite(txPin, txState);
    digitalWrite(ledPin, txState);
  }
  
}
