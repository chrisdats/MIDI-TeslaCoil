// Speaker test at this point
// Listens for MIDI messages and plays them on speaker

/* FirmwareMidiBoard.ino 
 Christopher Datsikas
 Date Created: 04-03-2016
 Date Modified: 04-03-2016
 Status: In Progress
 
 EQUIPMENT
 5-Pin Midi In Port
 6N138 Octocoupler
 Teensy 3.2
 IF-E96E 
 ULN2803A Darlington Transistor Array
 
 CONNECTIONS
 Teensy
 GND  -GND
 Pin00-RX1 (MIDI)
 PIN02-LED
 Pin03-LED
 Pin04-LED
 Pin06-OpticalTransmitter
 Pin07-RX3
 Pin08-TX3
 Pin09-RX2 (reserved)
 Pin10-Tx2 (reserved)
 Pin11-DOUT
 Pin12-DIN
 Pin13-SCK
 Pin20-CS
 
 
 ULN2803A Pin 1C/18  -> 330 Ohm Resistor -> IF-E96E Cathode (-) (short)
 IF-E96E Anode (+) (long) <-> VIN_3V3
 */


#include <MIDI.h>  // Add Midi Library

#define ledPin 13      // Teensy Board ledPin is on Pin 13
#define txPin   3              // Sinks IF-E96E

unsigned long t=0;  // timer for MIDI port inactivity

// Tx variables
float freq = 0.5;                // Change the frequency of the tone here (in Hz)
float period;
float duty = .50;               //Change the duty cycle of the tone here
int txState = LOW;
unsigned long ontime, offtime;
unsigned long currentMicros = 0;    // will overflow after 70 minutes
unsigned long previousMicros = 0;
unsigned long txInterval = offtime;

//Create an instance of the library using Teensy Hardware Serial Port 1 (not default)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
  pinMode(ledPin, OUTPUT);         // Set LED to output
  pinMode(txPin, OUTPUT);          // Set optical transmit pin ot out
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it 
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(MyHandleNoteOff); // This command tells the Midi Library 
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Need to open Serial Monitor
  }
  Serial.println("Serial Port Initialized");
}

void loop() { // Main loop
  // Continuously check if Midi data has been received
  if (MIDI.read()) {       // when a incoming MIDI message is received
    t = millis();                 // set timer
  }

  if (millis() - t > 10000) {
    t += 10000;
    Serial.println("(inactivity)");
  } 
}

// MyHandleNoteON is the function that will be calledPin by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) { 
  Serial.println(String("Note On:  ch=") + channel + ", note=" + pitch + ", velocity=" + velocity);
  digitalWrite(ledPin,HIGH);  //Turn ledPin on
}

// MyHandleNoteOFF is the function that will be calledPin by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOff(byte channel, byte pitch, byte velocity) {
  Serial.println(String("Note Off: ch=") + channel + ", note=" + pitch + ", velocity=" + velocity); 
  digitalWrite(ledPin,LOW);  //Turn ledPin off
}

void playMyNote(byte channel, byte pitch)
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
  }


