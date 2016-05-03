// Use this file to play input from keyboard or 5pin MIDI Device

/*
 MIDI_SimpleSynth.ino 
 Christopher Datsikas
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
 Pin08-TX3 - LCD
 */

#include <MIDI.h>
#include "noteList.h"
#include "pitches.h"


// define pins
#define greenLed  2
#define yellowLed 3
#define redLed    4
#define txOut     6

// for timer
unsigned long t=0;

//Create an instance of the library using Teensy Hardware Serial Port 1 (not default)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
static const unsigned sMaxNumNotes = 16;
MidiNoteList<sMaxNumNotes> midiNotes;


void handleNotesChanged(bool isFirstNote = false)
{
  if (midiNotes.empty())
  {
    noTone(txOut);
  }
  else
  {
    // Possible playing modes:
    // Mono Low:  use midiNotes.getLow
    // Mono High: use midiNotes.getHigh
    // Mono Last: use midiNotes.getLast

    byte currentNote = 0;
    if (midiNotes.getHigh(currentNote))
    {
      //Serial.print("here is tone: ");
      //Serial.println(sNotePitches[currentNote]);
      tone(txOut, sNotePitches[currentNote]);

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
  handleNotesChanged(firstNote);
}

// MyHandleNoteOFF is the function that will be called by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Note, and Velocity
void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
  midiNotes.remove(inNote);
  Serial.println(String("Note Off: ch=") + inChannel + ", note=" + inNote + ", velocity=" + inVelocity); 
  handleNotesChanged();
}

// -----------------------------------------------------------------------------

void setup()
{
  pinMode(yellowLed, OUTPUT);   // Set LED to output pin
  pinMode(txOut, OUTPUT);       // Set tx pin (audio or optical) to output
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it 
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(handleNoteOn); // This is important!! This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(handleNoteOff); // This command tells the Midi Library 
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.

  Serial.begin(9600);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Need to open Serial Monitor
  //  }
  Serial.println("playKeyboard.ino");
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

}

