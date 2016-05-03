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
#include <SdFat.h>
#include <MD_MIDIFile.h>
#include "pitches.h"

#define ledPin 3      // Teensy Board ledPin is on Pin 13
#define txPin   6              // Sinks IF-E96E

#define	DEBUG(x)	Serial.print(x)
#define	DEBUGX(x)	Serial.print(x, HEX)
#define	SERIAL_RATE	9600
#define  SD_SELECT  20
#define	ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

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
static const unsigned sAudioOutPin = 6;
static const unsigned sMaxNumNotes = 16;
MidiNoteList<sMaxNumNotes> midiNotes;

SdFat	SD;
MD_MIDIFile SMF;

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
char *tuneList[] = 
{
        "jingle.mid"
//	"LOOPDEMO.MID",  // simplest and shortest file
//        "ELISE.MID",
//	"TWINKLE.MID",
//	"GANGNAM.MID",
//	"FUGUEGM.MID",
//	"POPCORN.MID",
//	"AIR.MID",
//	"PRDANCER.MID",
//	"MINUET.MID",
//	"FIRERAIN.MID",
//	"MOZART.MID",
//	"FERNANDO.MID",
//	"SONATAC.MID",
//	"SKYFALL.MID",
//	"XMAS.MID",
//	"GBROWN.MID",
//	"PROWLER.MID",
//	"IPANEMA.MID",
//	"JZBUMBLE.MID",
};


// from SD_CARD_MIDIReading
void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
#if USE_MIDI
	if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
	{
		Serial.write(pev->data[0] | pev->channel);
		Serial.write(&pev->data[1], pev->size-1);
	}
	else
		Serial.write(pev->data, pev->size);
#endif
  DEBUG("\nM T");
  DEBUG(pev->track);
  DEBUG(":  Ch ");
  DEBUG(pev->channel+1);
  DEBUG(" Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
	DEBUGX(pev->data[i]);
    DEBUG(' ');
  }
  
   if (pev->data[0] >= 0x90) {
     const bool firstNote = midiNotes.empty();
     midiNotes.add(MidiNote(pev->data[1], pev->data[2]));
     handleNotesChanged(firstNote);
   }
   
   if (pev->data[0] >= 0x80) {
     midiNotes.remove(inNote);
     handleNotesChanged();
   }
}

void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs 
// to be processed through the midi communications interface. Most sysex events cannot 
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{
  DEBUG("\nS T");
  DEBUG(pev->track);
  DEBUG(": Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
    DEBUGX(pev->data[i]);
	DEBUG(' ');
  }
}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
	midi_event	ev;

	// All sound off
	// When All Sound Off is received all oscillators will turn off, and their volume
	// envelopes are set to zero as soon as possible.
	ev.size = 0;
	ev.data[ev.size++] = 0xb0;
	ev.data[ev.size++] = 120;
	ev.data[ev.size++] = 0;

	for (ev.channel = 0; ev.channel < 16; ev.channel++)
		midiCallback(&ev);
}


// from 5pin MIDI Input - SimpleSynth
void handleNotesChanged(bool isFirstNote = false)
{
    if (midiNotes.empty())
    {
        noTone(sAudioOutPin);
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
           Serial.print("here is tone");
           Serial.println(sNotePitches[currentNote]);
           tone(sAudioOutPin, sNotePitches[currentNote]);

        }
    }
}

// MyHandleNoteON is the function that will be calledPin by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    const bool firstNote = midiNotes.empty();
    midiNotes.add(MidiNote(inNote, inVelocity));
    Serial.print("adding ");
    Serial.print(inNote);
    Serial.print(" ");
    Serial.println(inVelocity);
    handleNotesChanged(firstNote);
}

// MyHandleNoteOFF is the function that will be calledPin by the Midi Library
// when a MIDI NOTE OFF message is received.
// * A NOTE ON message with Velocity = 0 will be treated as a NOTE OFF message *
// It will be passed bytes for Channel, Pitch, and Velocity
void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
    midiNotes.remove(inNote);
    handleNotesChanged();
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



void setup() {
  pinMode(ledPin, OUTPUT);         // Set LED to output
  pinMode(txPin, OUTPUT);          // Set optical transmit pin ot out
  
  // Initialize SD
  if (!SD.begin(SD_SELECT, SPI_FULL_SPEED))
  {
    DEBUG("\nSD init fail!");
    digitalWrite(SD_ERROR_LED, HIGH);
    while (true) ;
  }

  // Initialize MIDIFile
  SMF.begin(&SD);
  SMF.setMidiHandler(midiCallback);
  SMF.setSysexHandler(sysexCallback);
  
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Need to open Serial Monitor
  }
  Serial.println("Serial Port Initialized");
}

void playFile() {
  int  err;
	
	for (uint8_t i=0; i<ARRAY_SIZE(tuneList); i++)
	{
	  // reset LEDs
	  digitalWrite(READY_LED, LOW);
	  digitalWrite(SD_ERROR_LED, LOW);

	  // use the next file name and play it
    DEBUG("\nFile: ");
    DEBUG(tuneList[i]);
	  SMF.setFilename(tuneList[i]);
	  err = SMF.load();
	  if (err != -1)
	  {
		DEBUG("\nSMF load Error ");
		DEBUG(err);
		digitalWrite(SMF_ERROR_LED, HIGH);
		delay(WAIT_DELAY);
	  }
	  else
	  {
		// play the file
		while (!SMF.isEOF())
		{
			if (SMF.getNextEvent())
			tickMetronome();
		}

		// done with this one
		SMF.close();
		midiSilence();

		// signal finish LED with a dignified pause
		digitalWrite(READY_LED, HIGH);
		delay(WAIT_DELAY);
	  }
	}
}


void loop() { // Main loop
  playFile()
  
  // After file is over, begin reading in from keyboard
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it 
  // to respond to notes on channel 2 only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This command
  // tells the Midi Library which function you want to call when a NOTE ON command
  // is received. In this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(MyHandleNoteOff); // This command tells the Midi Library 
  // to call "MyHandleNoteOff" when a NOTE OFF command is received.
  
  
  // Continuously check if Midi data has been received
  if (MIDI.read()) {       // when a incoming MIDI message is received
    t = millis();                 // set timer
  }

  if (millis() - t > 10000) {
    t += 10000;
    Serial.println("(inactivity)");
  } 
}



