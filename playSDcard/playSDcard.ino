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
#define greenLed  2
#define yellowLed 3
#define redLed    4
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

SdFat	SD;
MD_MIDIFile SMF;
static const unsigned sMaxNumNotes = 16;
MidiNoteList<sMaxNumNotes> midiNotes;

const int pinOut = 6;

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
  if (midiNotes.empty())
  {
    //noTone(txOut);
    analogWrite(txOut, 0); 
  }
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
      //tone(txOut, sNotePitches[currentNote]);
      analogWriteFrequency(txOut, sNotePitches[currentNote]);
      analogWrite(txOut, 20); 

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

void setup(void)
{
  int  err;

  Serial.begin(SERIAL_RATE);

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
}
