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
float freq1 = 261;
float duty1 = 0.10;

float freq2 = 440;
float duty2 = 0.10;

//float freq = 100;                // Change the frequency of the tone here (in Hz)
float period1;
float period2;
//float duty = (1-.025);               //Change the duty cycle of the tone here
int txState1 = LOW;
unsigned long ontime1, offtime1;
unsigned long currentMicros1 = 0;    // will overflow after 70 minutes
unsigned long previousMicros1 = 0;
unsigned long txInterval1 = offtime1;

int txState2 = LOW;
unsigned long ontime2, offtime2;
unsigned long currentMicros2 = 0;    // will overflow after 70 minutes
unsigned long previousMicros2 = 0;
unsigned long txInterval2 = offtime2;

int txStateAll = LOW;

void setup() {
  // set the digital pin as output;
  pinMode(ledPin, OUTPUT);
  pinMode(txPin, OUTPUT);
  period1 = 1/freq1;                      // period is inverse of frequency (math)
  ontime1 = 1000000*period1*duty1;         // in us
  offtime1 = 1000000*period1*(1.0-duty1);  // in us

  period2 = 1/freq2;                      // period is inverse of frequency (math)
  ontime2 = 1000000*period2*duty2;         // in us
  offtime2 = 1000000*period2*(1.0-duty2);  // in us

  // for debugging. need to open Serial Monitor for program to run
  Serial.begin(9600);
  //while (!Serial);         // while the serial stream is not open, do nothing
  Serial.println(period1);
  Serial.println(ontime1);
  Serial.println(offtime1);
  Serial.println("Setup Complete");

  digitalWrite(txPin, LOW);
}

void loop()
{

  // same procedure as above but modification to allow for
  // different ontime and offtime
  currentMicros1 = micros();
  if (currentMicros1 - previousMicros1 > txInterval1) {
    previousMicros1 = currentMicros1;

    if (txState1 == LOW) {
      txState1 = HIGH;
      txInterval1 = ontime1;
    }
    else if (txState1 == HIGH) {
      txState1 = LOW;
      txInterval1 = offtime1;
    }
  }

  currentMicros2 = micros();
  if (currentMicros2 - previousMicros2 > txInterval2) {
    previousMicros2 = currentMicros2;

    if (txState2 == LOW) {
      txState2 = HIGH;
      txInterval2 = ontime2;
    }
    else if (txState2 == HIGH) {
      txState2 = LOW;
      txInterval2 = offtime2;
    }
  }
  txStateAll = txState1 || txState2;

  digitalWrite(txPin, txStateAll);
  digitalWrite(ledPin, txStateAll);


}

