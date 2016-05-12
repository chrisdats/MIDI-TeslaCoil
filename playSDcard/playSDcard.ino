// Use this file to play MIDI files from SD Card
// currently plays one file on repeats

/*
 MD_MIDIFile_Loop.ino 
 Christopher Datsikas
 Date Created: 04-03-2016
 Date Modified: 05-02-2016
 Status: In Progress

EQUIPMENT
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


#include <SdFat.h>
#include <MD_MIDIFile.h>
#include <MIDI.h>
#include "noteList.h"
#include "pitches.h"

// define pins
#define greenLED  2
#define yellowLED 3
#define redLED    4
#define txOut     6

#define DEBUGS(s)     Serial.print(s)
#define	DEBUG(s, x)	  { Serial.print(F(s)); Serial.print(x); }
#define	DEBUGX(s, x)	{ Serial.print(F(s)); Serial.print(x, HEX); }
#define	SERIAL_RATE	9600

// SD chip select pin for SPI comms.
#define  SD_SELECT  20

#define	ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
char *loopfile = "pirates.mid";
//char *loopfile = "nyan.mid"; //change to getHigh
//char *loopfile = "satisf.mid";

// on speaker use activeHigh
/// on Teslacoil use activeLow
boolean activeLow = false;
boolean polyphonic = true;

// variables for polyphonic support
int s = 0;
unsigned long time = 0;
float dutyCycle = 0.15;
boolean txStateAll = true;


SdFat	SD;
MD_MIDIFile SMF;
static const unsigned sMaxNumNotes = 16;
MidiNoteList<sMaxNumNotes> midiNotes;


void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
  DEBUG("\nM T", pev->track);
  DEBUG(":  Ch ", pev->channel+1);
  DEBUGS(" Data");
  for (uint8_t i=0; i<pev->size; i++)
  {
	  DEBUGX(" ", pev->data[i]);
  }
  // if it is a note on command
  if ((pev->data[0] == 0x90) && (pev->data[2] != 0)) {
     handleNoteOn(pev->channel+1, (pev->data[1]-20), pev->data[2]);
   }
   // if it is a note off command
   else if ((pev->data[0] == 0x80) || (pev->data[2] == 0)) {
     handleNoteOff(pev->channel+1, (pev->data[1]-20), pev->data[2]);
   } 
}

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
    if (midiNotes.getLast(currentNote))
    {
      //Serial.print("here is tone: ");
      //Serial.println(sNotePitches[currentNote]);
      analogWriteFrequency(txOut, sNotePitches[currentNote]);    // sets the frequency of the note
      analogWrite(txOut, dutyCycle*255);              // activates the note and sets the duty cycle
      digitalWrite(yellowLED, HIGH);        // set LED to HIGH when note is being played
    }
  }
}

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


void setup(void)
{
  int  err;

  Serial.begin(SERIAL_RATE);
  pinMode(yellowLED, OUTPUT);
  if (polyphonic == true) {
    pinMode(txOut, OUTPUT);
  }
  //analogWrite(txOut, 255);
  
  DEBUGS("\n[MidiFile Looper]");

  // Initialize SD
  if (!SD.begin(SD_SELECT, SPI_FULL_SPEED))
  {
    DEBUGS("\nSD init fail!");
    while (true) ;
  }

  // Initialize MIDIFile
  SMF.begin(&SD);
  SMF.setMidiHandler(midiCallback);
  SMF.looping(true);

  // use the next file name and play it
  DEBUG("\nFile: ", loopfile);
  SMF.setFilename(loopfile);
  err = SMF.load();
  if (err != -1)
  {
    DEBUG("\nSMF load Error ", err);
    while (true);
  }
}

void loop(void)
{
	// play the file
	if (!SMF.isEOF())
	{
		SMF.getNextEvent();
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
  
   Serial.println(txStateAll);
    digitalWrite(txOut, txStateAll);
    digitalWrite(yellowLED, txStateAll);
    
  }

}
