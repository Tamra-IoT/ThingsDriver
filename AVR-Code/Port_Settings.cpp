////////////////////////
#include "AVR-Code.h"

char digitalPointer = 0;
unsigned char digitalPin[DnIndex] = {'0'}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short digitalInterval[DnIndex] = {0};
char digitalMode[DnIndex] = {'0'};
unsigned char pinState[DnIndex] = {0}; // this reserved for the output pin states
unsigned short digitalTimer[DnIndex] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
///////////////////////////////////////////////////////////////
char analogPointer = 0;
unsigned char analogPin[AnIndex] = {'0'}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short analogInterval[AnIndex] = {0};
unsigned short analogTimer[AnIndex] = {0, 0, 0, 0, 0, 0};
//float analogFactor[AnIndex]={0};
char analogMode[AnIndex] = {'0'};
unsigned short pinAState[AnIndex] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // this reserved for the output Analog-pin states
///////////////////////////// variable to handle nodeMCU communication
const bool pwmPin[14] = {0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0};
////////////////////////


extern unsigned short ROMAddress;

void digitalSetup()
{
  char i = 0;
  //  Serial.println(F("from digital Handler"));
  //  while(digitalPin[i]!='0'){
  for (i = 0; i < digitalPointer; i++)
  {
    switch (digitalMode[i])
    {
    case 'i':
      pinMode(digitalPin[i], INPUT); //Serial.print(F("pinIn:"));  Serial.println(digitalPin[i]);
      break;
    case 'o':
      pinMode(digitalPin[i], OUTPUT); //digitalWrite(digitalPin[i], LOW);//pinState[digitalPin[i]]=0;//Serial.print(F("pinOut:"));Serial.println(digitalPin[i]);
      break;
    case 'p':
      pinMode(digitalPin[i], OUTPUT); //Serial.print(F("pinPWD:"));Serial.println(digitalPin[i]);
      break;
    default:; //Serial.println(F("undefined mode insde digital handlers"));
    }
  }
}

void analogSetup()
{
  char i = 0;
  //  Serial.println(F("from analog Handler"));
  //while(analogPin[i]!='0'){
  for (i = 0; i < analogPointer; i++)
  {
    /////////////////////////////////////////////////
    switch (analogMode[i])
    {
    case 'i':
      pinMode(analogPin[i], INPUT); //Serial.print(F("pinAIn:"));  Serial.println(digitalPin[i]);
      break;
    case 'o':
      pinMode(analogPin[i], OUTPUT); //digitalWrite(analogPin[i], LOW); pinAState[analogPin[i]-14]=0;//Serial.print(F("pinAOut:"));Serial.println(analogPin[i]);
      break;
    case 'a':
      pinMode(analogPin[i], INPUT); //Serial.print(F("pinA:"));Serial.println(analogPin[i]);
      break;
    default:; //Serial.println(F("undefined mode insde analog handlers"));
    }
    ////////////////////////////////////////////////
    //i++;
  }
}
////////////////////////
//The way we dynamic configure pins as shown below
//for(unsigned char i=1;i<=pins_number;i++)
//pin boolen
//bool pin[9]={1,1,1,1,1,1,1,1,1};
//{//Serial.println(i+4);
//  pinMode(i+4, OUTPUT);
//  digitalWrite((i+4),pin[i]);}

////////////////////////


void sendUFrame(char mode)
{
  unsigned char modes[2];
  if (mode == 'o' || mode == 'p')
  {
    modes[0] = 'o';
    modes[1] = 'p';
    mode = 'o';
  }
  else if (mode == 'i' || mode == 'a')
  {
    modes[0] = 'i';
    modes[1] = 'a';
    mode = 'i';
  } // here, we aggregate the modes that are defined on the cloud as input or outputs
  DynamicJsonDocument doc(250);
  doc["H"] = String(mode);
  //Serial.println(sizeof(modes));
  for (unsigned char j = 0; j < sizeof(modes); j++)
  {

    for (unsigned char i = 0; i < digitalPointer; i++)
    {
      if (modes[j] == digitalMode[i])
      {
        String Dn = "D";
        Dn += String(digitalPin[i]);
        //if(modes[j] == digitalMode[i]){doc[Dn] = pinState[digitalPin[i]];} // two fitler the by mode
        doc[Dn] = pinState[digitalPin[i]];
        digitalTimer[i] = 0; // these to work in the shortest time interval mode for each mode
        if (digitalTimer[i] >= digitalInterval[i])
        {
          digitalTimer[i] = 0;
        } // these to  sending only one frame for two ports or more that have the same time interval
      }
    }
    for (unsigned char i = 0; i < analogPointer; i++)
    {
      unsigned char pinNumber = analogPin[i] - 14;
      if (modes[j] == analogMode[i])
      {
        String An = "A";
        An += String(pinNumber);
        doc[An] = pinAState[pinNumber];
        analogTimer[i] = 0; // these to work in the shortest time interval mode for each mode
        //if(analogTimer[i]>= analogInterval[i]){analogTimer[i]=0;} // these to  sending only one frame for two ports or more that have the same time interval
      }
    }
  }
  serializeJson(doc, Serial);
  Serial.println();
}

void updatePortsStates()
{
  //  Serial.println(F("updatePortsStates called successfully"));
  for (char i = 0; i < digitalPointer; i++)
  {
    switch (digitalMode[i])
    {
    case 'i':
      pinState[digitalPin[i]] = digitalRead(digitalPin[i]); // Serial.print(F("pin:D"));Serial.print(digitalPin[i]); Serial.print(F("become=")); Serial.println( pinState[digitalPin[i]]);
      break;
    case 'o':
      digitalWrite(digitalPin[i], pinState[digitalPin[i]]); //Serial.print(F("pin:D"));Serial.print(digitalPin[i]); Serial.print(F("become=")); Serial.println( pinState[digitalPin[i]]);
      break;
    case 'p':
      analogWrite(digitalPin[i], pinState[digitalPin[i]]); //Serial.print(F("pinPWM:"));Serial.print(digitalPin[i]); Serial.print(F("become=")); ;Serial.println( pinState[digitalPin[i]]);
      break;
    }
  }
  /////////////////////////

  for (char i = 0; i < analogPointer; i++)
  {
    unsigned char pinNumber = analogPin[i] - 14;
    switch (analogMode[i])
    {
      //case 'o': digitalWrite(analogPin[i], pinAState[(analogPin[i]-14)]);Serial.print(F("pin:A"));Serial.print((analogPin[i]-14)); Serial.print(F("become=")); Serial.println( pinAState[analogPin[i]-14]);
    case 'o':
      digitalWrite(analogPin[i], pinAState[pinNumber]); //Serial.print(F("pin:A"));Serial.print(pinNumber); Serial.print(F("become=")); Serial.println( pinAState[pinNumber]);
      break;
    case 'i':
      pinAState[pinNumber] = digitalRead(analogPin[i]); // Serial.print(F("pin:A"));Serial.print(pinNumber); Serial.print(F("become=")); Serial.println( pinAState[pinNumber]);
      break;
    case 'a':
      pinAState[pinNumber] = analogRead(analogPin[i]); // Serial.print(F("pin:A"));Serial.print(pinNumber); Serial.print(F("become=")); Serial.println( pinAState[pinNumber]);
      break;
      //      case 'p': analogWrite(analogPin[i], pinState[digitalPin[i]]);Serial.print(F("pinPWM:"));Serial.print(digitalPin[i]-14); Serial.print(F("become=")); ;Serial.println( pinState[digitalPin[i]]);
      //      break;  // no pwm over Analog pin on the atmega328pa
    }
  }
}

void getSavedSettings()
{
  ROMAddress = 0;
  ROMAddress++;
  EEPROM.get(ROMAddress, digitalPointer);
  ROMAddress += sizeof(digitalPointer);
  EEPROM.get(ROMAddress, analogPointer);
  ROMAddress += sizeof(analogPointer);

  for (unsigned char i = 0; i < DnIndex; i++)
  {
    EEPROM.get(ROMAddress, pinState[i]);
    ROMAddress += sizeof(pinState[i]);
  }
  for (unsigned char i = 0; i < AnIndex; i++)
  {
    EEPROM.get(ROMAddress, pinAState[i]);
    ROMAddress += sizeof(pinAState[i]);
  }

  for (char i = 0; i < digitalPointer; i++)
  {
    EEPROM.get(ROMAddress, digitalPin[i]);
    ROMAddress += sizeof(digitalPin[i]);
    EEPROM.get(ROMAddress, digitalInterval[i]);
    ROMAddress += sizeof(digitalInterval[i]);
    EEPROM.get(ROMAddress, digitalMode[i]);
    ROMAddress += sizeof(digitalMode[i]);
    //    EEPROM.get(ROMAddress, digitalTimer[i] ); ROMAddress+=sizeof(digitalTimer[i]);
  }

  for (char i = 0; i < analogPointer; i++)
  {
    EEPROM.get(ROMAddress, analogPin[i]);
    ROMAddress += sizeof(analogPin[i]);
    EEPROM.get(ROMAddress, analogInterval[i]);
    ROMAddress += sizeof(analogInterval[i]);
    EEPROM.get(ROMAddress, analogMode[i]);
    ROMAddress += sizeof(analogMode[i]);
    //    EEPROM.get(ROMAddress, analogTimer[i] ); ROMAddress+=sizeof(analogTimer[i]);
  }
}

void updateSettings()
{
  //  for(char i; i <AnIndex; i++ ){pinAState[i]=0;}
  ROMAddress = 0;
  EEPROM.update(ROMAddress, 1); //this is the write flag to use it as indication that the board has a saved configuration
  ROMAddress++;
  EEPROM.update(ROMAddress, digitalPointer);
  ROMAddress += sizeof(digitalPointer);
  EEPROM.update(ROMAddress, analogPointer);
  ROMAddress += sizeof(analogPointer);

  for (char i = 0; i < DnIndex; i++)
  {
    EEPROM.update(ROMAddress, pinState[i]);
    ROMAddress += sizeof(pinState[i]);
  }
  for (char i = 0; i < AnIndex; i++)
  {
    EEPROM.update(ROMAddress, pinAState[i]);
    ROMAddress += sizeof(pinAState[i]);
  }

  for (char i = 0; i < digitalPointer; i++)
  {
    EEPROM.update(ROMAddress, digitalPin[i]);
    ROMAddress += sizeof(digitalPin[i]);
    EEPROM.update(ROMAddress, digitalInterval[i]);
    ROMAddress += sizeof(digitalInterval[i]);
    EEPROM.update(ROMAddress, digitalMode[i]);
    ROMAddress += sizeof(digitalMode[i]);
    //    EEPROM.update(ROMAddress, digitalTimer[i] ); ROMAddress+=sizeof(digitalTimer[i]);
  }

  for (char i = 0; i < analogPointer; i++)
  {
    EEPROM.update(ROMAddress, analogPin[i]);
    ROMAddress += sizeof(analogPin[i]);
    EEPROM.update(ROMAddress, analogInterval[i]);
    ROMAddress += sizeof(analogInterval[i]);
    EEPROM.update(ROMAddress, analogMode[i]);
    ROMAddress += sizeof(analogMode[i]);
    //    EEPROM.update(ROMAddress, analogTimer[i] ); ROMAddress+=sizeof(analogTimer[i]);
  }
}
