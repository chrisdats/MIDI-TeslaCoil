// MIDI Input Test that utilizes callbacks found in MIDI library v4.2
// prints out to serial monitor note info and lights up on board led
// when note is played

#include <MIDI.h>  // Add Midi Library

#define LED 13    // Teensy Board LED is on Pin 13
unsigned long t=0;

//Create an instance of the library using Teensy Hardware Serial Port 1 (not default)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
  pinMode (LED, OUTPUT); // Set Teensy board pin 13 to output (onboard LED)
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it 
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This is important!! This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(MyHandleNoteOff); // This command tells the Midi Library 
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Need to open Serial Monitor
  }
  Serial.println("MIDI Input Test");
}

void loop() { // Main loop
  // Continuously check if Midi data has been received
  if (MIDI.read()) {       // when a incoming MIDI message is received
    t = millis();                 // set timer to 0
  }

  if (millis() - t > 10000) {
    t += 10000;
    Serial.println("(inactivity)");
  } 
}

// MyHandleNoteON is the function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) { 
  Serial.println(String("Note On:  ch=") + channel + ", note=" + pitch + ", velocity=" + velocity);
  digitalWrite(LED,HIGH);  //Turn LED on
}

// MyHandleNoteOFF is the function that will be called by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOff(byte channel, byte pitch, byte velocity) {
  Serial.println(String("Note Off: ch=") + channel + ", note=" + pitch + ", velocity=" + velocity); 
  digitalWrite(LED,LOW);  //Turn LED off
}


