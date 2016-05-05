/***********************************************************************
SerSerial3 Example Code
SparkFun Electronics 
Joel Bartlett
December 20, 2012 

This code uses the information presented in the SerSerial3 Datasheet 
to create an Arduino example using the SerSerial3 from SparkFun Electonics. 
Each of the SerSerial3's capabilities is broken up into seperate functions
within the sketch. Simply call each function with the correct parameters 
to get the desired result form the Serial3 screen. 

This code was devoloped for the Arduino IDE v102

To use, connect the following pins
VDD -> 5V
GND -> GND
Serial3 RX -> Arduino TX (pin 1)

***Don't forgect to disconnect the Serial3's RX pin from the TX pin of 
the Arduino's UART line while programming!***

You can also check out the SerSerial3 library from Arduino 
http://playground.arduino.cc/Code/SerSerial3

*If you need to reprogram often, or need the UART for another device,
you can use the Software Serial libary to create a 
seperate UART for the Serial3.
Arduino IDE -> Skecth -> Import Library ->Software Serial

To declare a new UART using SoftwareSerial, insert this line:
SoftwareSerial NAME(x,y); // RX, TX
where Name is the name of the new UART, x is the RX pin, and y is the TX pin.

"THE BEER-WARE LICENSE"
As long as you retain this notice you can do whatever you want with this stuff. 
If we meet some day, and you think this stuff is worth it, you can buy me a beer.
************************************************************************/


//-------------------------------------------------------------------------------------------
void setup()
{
Serial3.begin(9600);// all SerSerial3s come at 9600 Baud by default
}
//-------------------------------------------------------------------------------------------
void loop()
{
  scrollingMarquee();
  tempAndHumidity();
  counter();
  backlight();
  cursors();
}
//-------------------------------------------------------------------------------------------
void clearScreen()
{
  //clears the screen, you will use this a lot!
  Serial3.write(0xFE);
  Serial3.write(0x01); 
}
//-------------------------------------------------------------------------------------------
void selectLineOne()
{ 
  //puts the cursor at line 0 char 0.
  Serial3.write(0xFE); //command flag
  Serial3.write(128); //position
}
//-------------------------------------------------------------------------------------------
void selectLineTwo()
{ 
  //puts the cursor at line 0 char 0.
  Serial3.write(0xFE); //command flag
  Serial3.write(192); //position
}
//-------------------------------------------------------------------------------------------
void moveCursorRightOne()
{
  //moves the cursor right one space
  Serial3.write(0xFE); //command flag
  Serial3.write(20); // 0x14
}
//-------------------------------------------------------------------------------------------
void moveCursorLeftOne()
{
  //moves the cursor left one space
  Serial3.write(0xFE); //command flag
  Serial3.write(16); // 0x10
}
//-------------------------------------------------------------------------------------------
void scrollRight()
{
  //same as moveCursorRightOne
  Serial3.write(0xFE); //command flag
  Serial3.write(20); // 0x14
}
//-------------------------------------------------------------------------------------------
void scrollLeft()
{
  //same as moveCursorLeftOne
  Serial3.write(0xFE); //command flag
  Serial3.write(24); // 0x18
}
//-------------------------------------------------------------------------------------------
void turnDisplayOff()
{
  //this tunrs the display off, but leaves the backlight on. 
  Serial3.write(0xFE); //command flag
  Serial3.write(8); // 0x08
}
//-------------------------------------------------------------------------------------------
void turnDisplayOn()
{
  //this turns the dispaly back ON
  Serial3.write(0xFE); //command flag
  Serial3.write(12); // 0x0C
}
//-------------------------------------------------------------------------------------------
void underlineCursorOn()
{
  //turns the underline cursor on
  Serial3.write(0xFE); //command flag
  Serial3.write(14); // 0x0E
}
//-------------------------------------------------------------------------------------------
void underlineCursorOff()
{
  //turns the underline cursor off
  Serial3.write(0xFE); //command flag
  Serial3.write(12); // 0x0C
}
//-------------------------------------------------------------------------------------------
void boxCursorOn()
{
  //this turns the box cursor on
  Serial3.write(0xFE); //command flag
  Serial3.write(13); // 0x0D
}
//-------------------------------------------------------------------------------------------
void boxCursorOff()
{
  //this turns the box cursor off
  Serial3.write(0xFE); //command flag
  Serial3.write(12); // 0x0C
}
//-------------------------------------------------------------------------------------------
void toggleSplash()
{
  //this toggles the spalsh screenif off send this to turn onif on send this to turn off
  Serial3.write(0x7C); //command flag = 124 dec
  Serial3.write(9); // 0x09
}
//-------------------------------------------------------------------------------------------
int backlight(int brightness)// 128 = OFF, 157 = Fully ON, everything inbetween = varied brightnbess 
{
  //this function takes an int between 128-157 and turns the backlight on accordingly
  Serial3.write(0x7C); //NOTE THE DIFFERENT COMMAND FLAG = 124 dec
  Serial3.write(brightness); // any value between 128 and 157 or 0x80 and 0x9D
}
//-------------------------------------------------------------------------------------------
void scrollingMarquee()
{
//This function scroll text across the screen on both lines
  clearScreen(); // it's always good to clear the screen before movonh onto a new print
  for(int j = 0; j < 17; j++)
  {
    selectLineOne();
    for(int i = 0; i < j;i++)
      moveCursorRightOne();
    Serial3.print("SPARK");
    selectLineTwo();
    for(int i = 0; i < j;i++)
      moveCursorRightOne();
    Serial3.print(" FUN");
    delay(500); // you must have a delay, otherwise the screen will print and clear before you can see the text
    clearScreen();
  }
}
//-------------------------------------------------------------------------------------------
void counter()
{
  //this function prints a simple counter that counts to 10
  clearScreen();
  for(int i = 0; i <= 10; i++)
  {
    Serial3.print("Counter = ");
    Serial3.print(i, DEC);
    delay(500);
    clearScreen();
  }
}
//-------------------------------------------------------------------------------------------
void tempAndHumidity()
{
  //this function shows how you could read the data from a temerature and humidity 
  //sensro and then print that data to the SerSerial3.
  
  //these could be varaibles instead of static numbers 
  float tempF = 77.0; 
  float tempC = 25.0;
  float humidity = 67.0;
  
  clearScreen();
  selectLineOne();
  Serial3.print(" Temp = ");
  Serial3.print((long)tempF, DEC);
  Serial3.print("F ");
  Serial3.print((long)tempC, DEC);
  Serial3.print("C");
  selectLineTwo();
  Serial3.print(" Humidity = ");
  Serial3.print((long)humidity, DEC); 
  Serial3.print("%");
  delay(2500);
}
//-------------------------------------------------------------------------------------------
void backlight()
{
  //this function shows the different brightnesses to which the backlight can be set 
  clearScreen();
  for(int i = 128; i < 158; i+=2)// 128-157 are the levels from off to full brightness
  {
    backlight(i);
    delay(100);
    Serial3.print("Backlight = ");
    Serial3.print(i, DEC);
    delay(500);
    clearScreen();
  }
}
//-------------------------------------------------------------------------------------------
void cursors()
{
  //this function shows the different cursors avaiable on the SerSerial3
  clearScreen();
  
  boxCursorOn();
  Serial3.print("Box On");
  delay(1500);
  clearScreen();
  
  boxCursorOff();
  Serial3.print("Box Off");
  delay(1000);
  clearScreen();
  
  underlineCursorOn();
  Serial3.print("Underline On");
  delay(1500);
  clearScreen();
  
  underlineCursorOff();
  Serial3.print("Underline Off");
  delay(1000);
  clearScreen();
}
