void setup() {
  Serial1.begin(31250);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("MIDI Input Test");
}

int incomingByte;
void loop() {
  if (Serial1.available()) {
    incomingByte = Serial1.read();
    Serial.println(incomingByte);
  }
}
