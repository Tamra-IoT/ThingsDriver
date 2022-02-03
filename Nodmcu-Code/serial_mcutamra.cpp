#include "serial_mcutamra.h"
bool stringComplete = false;
String inputString = ""; 
void serialEvent()
{
  if (Serial.available())
  {
    inputString = ""; //here we freeup inputString varabile becuause some frame would send without any need to change or transelate to json
    while (Serial.available())
    {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      if (inChar != ' ')
        inputString += inChar;
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      if (inChar == '\n')
      {
        stringComplete = true;
      }
      delay(5);
    }
  }
}
