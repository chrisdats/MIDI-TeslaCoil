// Use this file to play input from keyboard or 5pin MIDI Device

/*
 playKeyboard.ino 
 Christopher Datsikas
 Status: Works
 
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
 Pin08-TX3 - LCD
 */

#include <MIDI.h>
#include "noteList.h"
#include "pitches.h"


// define pins
#define greenLED  2
#define yellowLED 3
#define redLED    4
#define txOut     6

// on speaker use activeHigh
/// on Teslacoil use activeLow
boolean activeLow = false;
boolean polyphonic = true;

// for timer
unsigned long t = 0;

// variables for polyphonic support
int s = 0;
unsigned long time = 0;
float dutyCycle = 0.15;
boolean txStateAll = false;


//Create an instance of the library using Teensy Hardware Serial Port 1 (not default)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
static const unsigned sMaxNumNotes = 16;
MidiNoteList<sMaxNumNotes> midiNotes;


void handleNotesChanged(bool isFirstNote = false)
{
  // if there is no note, play nothing
  if (midiNotes.empty())
  {
    if (activeLow == true) {
      analogWrite(txOut, 255);
    }
    else {
      analogWrite(txOut, 0);
    }
    digitalWrite(yellowLED, LOW); // set LED to OFF when note is NOT being played
  }
  // if there is a note in memory play it
  else
  {
    // Possible playing modes:
    // Mono Low:  use midiNotes.getLow
    // Mono High: use midiNotes.getHigh
    // Mono Last: use midiNotes.getLast
    
    byte currentNote = 0;
    if (midiNotes.getHigh(currentNote))
    {
      analogWriteFrequency(txOut, sNotePitches[currentNote]);    // sets the frequency of the note
      analogWrite(txOut, 120);              // activates the note and sets the duty cycle
      digitalWrite(yellowLED, HIGH);        // set LED to HIGH when note is being played
    }
  }
}

// -----------------------------------------------------------------------------
// MyHandleNoteON is the function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Note, and Velocity
void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
  const bool firstNote = midiNotes.empty();
  midiNotes.add(MidiNote(inNote, inVelocity));
  Serial.println(String("Note On:  ch=") + inChannel + ", note=" + inNote + ", velocity=" + inVelocity);
  // polyphonic handlesNotes differently
  if (polyphonic == false) {
    handleNotesChanged(firstNote);
  }
}

// MyHandleNoteOFF is the function that will be called by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Note, and Velocity
void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
  midiNotes.remove(inNote);
  Serial.println(String("Note Off: ch=") + inChannel + ", note=" + inNote + ", velocity=" + inVelocity); 
  // polyphonic handles notes differently
  if (polyphonic == false) {
    handleNotesChanged();
  }
}

// checks if the note should be in its ON state or OFF state
// takes as arguments: currentTime (as micros), period (as micros), dutyCycle (as decmial fraction)
int checkNote(unsigned long currentTime, int period, float dutyCycle) {
  int phase = (currentTime) % period;
  if (phase < period*dutyCycle) {
    return 1; // note should be on
  }
  else if (phase > period*dutyCycle) {
    return 0; // note should be off
  }
}


// -----------------------------------------------------------------------------

void setup()
{
  pinMode(yellowLED, OUTPUT);   // Set LED to output pin
  pinMode(txOut, OUTPUT);       // Set tx pin (audio or optical) to output
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it 
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(handleNoteOn); // This is important!! This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(handleNoteOff); // This command tells the Midi Library 
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.

  // Begin Serial communication
  Serial.begin(9600);    // Serial communication to computer
  Serial3.begin(9600);  // Serial communication to LCD screen
  delay(500);          // wait for serial communication to be established.
  
  // LCD Setup
  //clears the screen
  Serial3.write(0xFE);  //command flag
  Serial3.write(0x01); 
  //puts the cursor at line 0 char 0.
  Serial3.write(0xFE); //command flag
  Serial3.write(128); //position
  // slightly dim backlight
  Serial3.write(0x7C); // NOTE THE DIFFERENT COMMAND FLAG = 124 dec
  Serial3.write(150); // brightness - any value between 128 and 157 or 0x80 and 0x9D
  delay(1);
  Serial3.print("Keyboard");
  delay(1);
  
  // start by setting output to OFF
  if (activeLow == true) {
    analogWrite(txOut, 255);
  }
  else
    analogWrite(txOut, 0);
}


void loop()
{
  // Continuously check if Midi data has been received
  if (MIDI.read()) {       // when a incoming MIDI message is received
    t = millis();                 // set timer to 0
  }

  if (millis() - t > 10000) {
    t += 10000;
    Serial.println("(inactivity)");
  }
  
  if (polyphonic == true) {
    s = 0;           // reset sum to zero
    time = micros();  // save current time
    // for all notes in noteList
    for (int i = 0; i < midiNotes.size(); i++) {
      byte currentNote = 0;
      if (midiNotes.get(i, currentNote)) {
        s += checkNote(time, 1000000/sNotePitches[currentNote], dutyCycle);
      }
    }
    if (s > 0) {
      txStateAll = true; // on
    }
    else {
      txStateAll = false; //off
    }
  
    digitalWrite(txOut, txStateAll);
    digitalWrite(yellowLED, txStateAll);
    
  }
  
  
}

