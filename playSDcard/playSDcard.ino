// Use this file to play MIDI files from SD Card

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
#include "pitches.h"

#define	USE_MIDI	0

#if USE_MIDI // set up for direct MIDI serial output

#define DEBUGS(s)
#define	DEBUG(s, x)
#define	DEBUGX(s, x)
#define	SERIAL_RATE	31250

#else // don't use MIDI to allow printing debug statements

#define DEBUGS(s)     Serial.print(s)
#define	DEBUG(s, x)	  { Serial.print(F(s)); Serial.print(x); }
#define	DEBUGX(s, x)	{ Serial.print(F(s)); Serial.print(x, HEX); }
#define	SERIAL_RATE	9600

#endif // USE_MIDI


// SD chip select pin for SPI comms.
// Arduino Ethernet shield, pin 4.
// Default SD chip select is the SPI SS pin (10).
// Other hardware will be different as documented for that hardware.
#define  SD_SELECT  20

#define	ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
char *loopfile = "pirates.mid";  // simple and short file

SdFat	SD;
MD_MIDIFile SMF;

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
  if ((pev->data[0] == 0x90) && (pev->data[2] != 0)) {
     tone(pinOut, sNotePitches[(pev->data[1]-20)]);
   }
   else if ((pev->data[0] == 0x80) || (pev->data[2] == 0)) {
     noTone(pinOut);
   }
  
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
