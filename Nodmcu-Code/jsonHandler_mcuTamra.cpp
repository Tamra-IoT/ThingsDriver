#include"jsonHandler_mcuTamra.h"

extern String inputString;     // a String to hold incoming data
extern bool stringComplete;

extern bool Applied;

bool debugUpdate = false;
bool startReceivingSetting =true;
bool receivedCloudSettings = false;
bool SendingOutputUpdate = false;
bool SendingInputUpdate = false;
bool SettingFromAVR = false;
bool receivedSetting = false;
bool receivedCommand = false;
/////////////////////////////////////////////////////////////////
unsigned char digitalPointer = 0;
unsigned char digitalPin[12] = {2, 3, 4, 5, 6, 7, 13}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short digitalInterval[12] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
char digitalMode[12] = {'i', 'i', 'i', 'o', 'o', 'o'};
unsigned char pinState[14] = {1, 3, 5, 7, 8, 9, 4}; // this reserved for the output pin states
unsigned short digitalTimre[12] = {1, 66, 44, 22, 11, 88, 4};
/////////////////////////////////////////////////////////////////
unsigned char analogPointer = 0;
unsigned char analogPin[6] = {1, 2, 3, 4}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short analogInterval[6] = {10, 20, 30, 40, 50, 60};
unsigned short analogTimre[6] = {20, 5, 3, 9, 45};
//float analogFactor[6]={0};
char analogMode[6] = {'a', 'a', 'a', 'a', 'i'};
unsigned short pinAState[6] = {20, 21, 20, 20, 2120, 21}; // this reserved for the output Analog-pin states
//issues 1- I write this part on the nodemcu code receivingSettingfromCloud a stackoverflow happend==> solved by adding if no err in the recieved frame
const bool pwmPin[14] = {0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0}; // unpdate this part to disable PWM mode for Pin 9 and 10 due to Timer 1 confilct

void sendingSettingtoCloud()
{
  inputString = "";
  DynamicJsonDocument doc(4000);

  for (unsigned char i = 2; i < DnIndex; i++)
  {
    String Dn = "D";
    Dn += String(i);
    JsonObject D_Config = doc.createNestedObject(Dn);
    JsonArray D_modes = D_Config.createNestedArray("modes");
    //D_modes.add("a");
    D_modes.add("i");
    D_modes.add("o");
    D_Config["mode"] = NULL;
    D_Config["int"] = NULL; //// there is no pwd in
    if (pwmPin[i])
      D_modes.add("p");
  }

  for (char i = 0; i < digitalPointer; i++)
  {
    String Dn = "D";
    Dn += String(digitalPin[i]);
    doc[Dn]["mode"] = String(digitalMode[i]);
    doc[Dn]["int"] = digitalInterval[i]; //// there is no pwd in
  }

  for (unsigned char i = 0; i < AnIndex; i++)
  {
    String An = "A";
    An += String(i);
    JsonObject A_Config = doc.createNestedObject(An);
    JsonArray A_modes = A_Config.createNestedArray("modes");
    A_modes.add("a");
    A_modes.add("i");
    A_modes.add("o");
    A_Config["mode"] = NULL;
    A_Config["int"] = NULL; //// there is no pwd in
  }

  for (char i = 0; i < analogPointer; i++)
  {
    String An = "A";
    An += String(analogPin[i] - 14);
    doc[An]["mode"] = String(analogMode[i]);
    doc[An]["int"] = analogInterval[i]; //// there is no pwd in
  }

  doc["_applied"] = int(Applied);
  serializeJson(doc, inputString);
  Serial.println();
}

///////////////////////////////////////


void receivingSettingfromCloud()
{
  digitalPointer = 0;
  analogPointer = 0;
  DynamicJsonDocument doc(4000);

  ///////////////////////////////////////////////////////////// catch any error during serialization
  DeserializationError err = deserializeJson(doc, inputString);
  if (err)
  {
    //Serial.print(F("deserializeJson() failed with code "));
    //Serial.println(err.c_str());
  }
  if (!err)
  {
    if (!(doc["_applied"]))
    {
      ///////////////////////////////////////////////////
      JsonObject frame = doc.as<JsonObject>();
      for (JsonPair keyValue : frame)
      {
        String item = keyValue.key().c_str();
        if (item.substring(0, 1) == "D" && (doc[item]["mode"]))
        { //Digital pin hadler
          String modePin = doc[item]["mode"]; // "o"
          ////////////////////////////////////
          digitalPin[digitalPointer] = item.substring(1).toInt();
          digitalInterval[digitalPointer] = doc[item]["int"];
          digitalMode[digitalPointer] = modePin.charAt(0);
          digitalPointer++;
        }
        else if (item.substring(0, 1) == "A" && doc[item]["mode"] != 0)
        { //Analog pin hadler
          String modePin = doc[item]["mode"];
          analogPin[analogPointer] = item.substring(1).toInt() + 14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
          analogInterval[analogPointer] = doc[item]["int"];
          analogMode[analogPointer] = modePin.charAt(0);
          analogPointer++;
          //////////////////////////////////
        }
        else if (item.substring(0, 1) == "_")
        {
          Applied = doc[item];
        }
        receivedCloudSettings = true;
        debugUpdate = true; // to print the received setting
      }
    }
    else
    {
      receivedCloudSettings = false;
    }
  }
}





///////////////////////////////////////////////////AVR Handler
void AVRsaySomething()
{
  DynamicJsonDocument doc(1000); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
  ///////////////////////////////////////////////////////////// catch any error during serialization
  DeserializationError err = deserializeJson(doc, inputString);
  if (err)
  {
    //Serial.print(F("deserializeJson() failed with code "));
    //Serial.println(err.c_str());
  }
  if (!err)
  { //if  error discard the recieved frame,and if the frame is a complete Json file start handle
    ///////////////////////////////////////////////////
    JsonObject frame = doc.as<JsonObject>();
    String frameType = doc["H"]; // applying best practice with char pointer
    // Serial.print(F("frameType:"));
    // Serial.println(frameType);
    if (frameType == "s")
    {
      SettingFromAVR = true;
    }
    else if (frameType == "o")
    {
      SendingOutputUpdate = true;
    }
    else if (frameType == "i")
    {
      SendingInputUpdate = true;
    }
  }
}
////////////////////////////////////////////////////////////////
void ReceiveSettingFromAVR()
{
  DynamicJsonDocument doc(1000); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
  ////////////////////////////////
  digitalPointer = 0;
  analogPointer = 0; // This when the AVR start sending the setting we have to rest the analog and digital pointers
  Applied = false;
  while (!Applied)
  {
    DeserializationError err = deserializeJson(doc, inputString);
    if (err)
    {
      //      Serial.print(F("deserializeJson() failed with code "));
      //      Serial.println(err.c_str());
    }
    if (!err)
    { //if  error discard the recieved frame,and if the frame is a complete Json file start handle
      ///////////////////////////////////////////////////
      JsonObject frame = doc.as<JsonObject>();
      String frameType = doc["H"]; // applying best practice with char pointer
      for (JsonPair keyValue : frame)
      {
        String item = keyValue.key().c_str();
        //////////////////////////////////////////////////////
        if (item.substring(0, 1) == "D")
        { //Digital pin hadler
          //////////////////////////////////////////////////
          String modePin = doc[item]["m"]; // "o"
          ////////////////////////////////////
          digitalPin[digitalPointer] = item.substring(1).toInt();
          digitalInterval[digitalPointer] = doc[item]["i"];
          digitalMode[digitalPointer] = modePin.charAt(0);
          digitalPointer++;
          receivedCommand = true; // incase the user send an command before define the pin as output so by add this line the system will excute the saved states
          debugUpdate = true;     // to print the received setting
        }
        else if (item.substring(0, 1) == "A")
        {
          String modePin = doc[item]["m"];
          analogPin[analogPointer] = item.substring(1).toInt() + 14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
          analogInterval[analogPointer] = doc[item]["i"];
          analogMode[analogPointer] = modePin.charAt(0);
          analogPointer++;
          debugUpdate = true; // to print the received setting
          //////////////////////////////////
        }
        else if (item.substring(0, 1) == "_")
        {
          Applied = true;
          startReceivingSetting = true;
        }
      }
    }
    inputString = "";
    if (!Applied)
    {
      stringComplete = false;
    }
    while (!stringComplete)
    {
      serialEvent();
    }
  }
}
////////////////////////////////////////////////////////////////
void sendingSettingsToAVR()
{

  DynamicJsonDocument doc(200); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out

  for (unsigned char i = 0; i < digitalPointer; i++)
  {
    doc["H"] = "s";
    String Dn = "D";
    Dn += String(digitalPin[i]);
    doc["H"] = "s";
    JsonObject D_Config = doc.createNestedObject(Dn);
    doc[Dn]["m"] = String(digitalMode[i]);
    doc[Dn]["i"] = digitalInterval[i]; //// there is no pwd in

    serializeJson(doc, Serial);
    Serial.println();
    doc.clear();
    delay(200);
  }

  for (unsigned char i = 0; i < analogPointer; i++)
  {
    doc["H"] = "s";
    String An = "A";
    An += String(analogPin[i] - 14);
    JsonObject A_Config = doc.createNestedObject(An);
    doc[An]["m"] = String(analogMode[i]);
    doc[An]["i"] = analogInterval[i]; //// there is no pwd in

    serializeJson(doc, Serial);
    Serial.println();
    doc.clear();
    delay(200);
  }

  doc["H"] = "s";
  doc["_applied"] = int(Applied);
  serializeJson(doc, Serial);
  inputString = "";
  Serial.println();
}

/////////////////////////////////////////////////////////

void sendingCommandsToAVR()
{
  DynamicJsonDocument doc(400);
  DynamicJsonDocument AVRdoc(400);
  AVRdoc["H"] = "c";
  ///////////////////////////////////////////////////////////// catch any error during serialization
  DeserializationError err = deserializeJson(doc, inputString);
  if (err)
  {
    //    Serial.print(F("deserializeJson() failed with code "));
    //    Serial.println(err.c_str());
  }
  /////////////////////////////////////////////////////////////
  if (!err)
  {
    JsonObject outdoc = doc["out"].as<JsonObject>();
    for (JsonPair keyValue : outdoc)
    {
      String key = keyValue.key().c_str();
      AVRdoc[key] = doc["out"][key];
    }
    if (!(doc["in"].isNull()))
    {
      AVRdoc["in"] = doc["in"];
    }
    serializeJson(AVRdoc, Serial);
    Serial.println();
  }
}
