testReadme.txt
Christopher Datsikas
Apr 2 2016

Use the code in this folder to seperately test
each subsystem in MIDI Interupter Board. If
starting from scratch the following sequence
is recommended.

1. teensyBlink.ino - use this file to verify proper
installation of TeensyIDE addon, functioning Teensy,
and succesful connection between computer and Teensy

2. opticalTransmitterTest.ino - use this file to 
verify proper hookup and operation of the optical
transmitter

3.a) ReadWriteSdFat.ino
  b) SdInfo.ino
Both files straight from SdFat library
examples. Use to very that SdFat library 
is installed properly and that SD card connection
is good. 

4. a) MIDI_Input_Test.ino - use to verify MIDI library
works and that MIDI notes from 5 pin connector are being
recognized by Teensy
   b) MIDI_Input_Test2.ino - use to verify MIDI library
 event handlers work
     (MIDI_Input_Bytes.ino is for debugging only)