#include "Tamra_Serial.h"

extern String inputString;     // a String to hold incoming data
extern bool stringComplete;
void serialEvent()
{
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar != ' ')
      inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    delay(5); // comment this line if you chose 9600 baudrade in case of using 115200 this should included
    if (inChar == '\n')
    {
      stringComplete = true;
    }
  }
}
