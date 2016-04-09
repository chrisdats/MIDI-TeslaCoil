#include <MIDI.h>
#include "noteList.h"
#include "pitches.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// This example shows how to make a simple synth out of an Arduino, using the
// tone() function. It also outputs a gate signal for controlling external
// analog synth components (like envelopes).

static const unsigned sAudioOutPin = 6;
static const unsigned sMaxNumNotes = 16;
MidiNoteList<sMaxNumNotes> midiNotes;



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

// -----------------------------------------------------------------------------

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

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
    midiNotes.remove(inNote);
    handleNotesChanged();
}

// -----------------------------------------------------------------------------

void setup()
{
    pinMode(sAudioOutPin, OUTPUT);
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin();
}

void loop()
{
    MIDI.read();
}
