#include "JSON_Handler.h"
/////////////////////////////////////////////////

extern char digitalPointer;
extern unsigned char digitalPin[DnIndex]; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
extern unsigned short digitalInterval[DnIndex];
extern char digitalMode[DnIndex];
extern unsigned char pinState[DnIndex]; // this reserved for the output pin states
extern unsigned short digitalTimer[DnIndex];
///////////////////////////////////////////////////////////////
extern char analogPointer;
extern unsigned char analogPin[AnIndex]; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
extern unsigned short analogInterval[AnIndex];
extern unsigned short analogTimer[AnIndex];
//float analogFactor[AnIndex];
extern char analogMode[AnIndex];
extern unsigned short pinAState[AnIndex]; // this reserved for the output Analog-pin states
///////////////////////////// variable to handle nodeMCU communication
extern const bool pwmPin[14];
////////////////////////

extern String inputString;     // a String to hold incoming data

///////////////////////
extern bool startReceivingSetting;
/////////

extern bool Applied;
extern bool receivedCommand;
extern bool sendOutputFrame;
extern bool sendInputFrame;

/////////////////////////////////////////////////
void jsonHandler()
{
  DynamicJsonDocument doc(500); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
  //DynamicJsonDocument doc(300); // this in case we use dynamic allocation to free the memory, here code accepts around 10 in/out
  //DynamicJsonDocument smallDoc(50);
  //StaticJsonDocument<550> doc;
  //StaticJsonDocument<50> smallDoc;
  ///////////////////////////////////////////////////////////// catch any error during serialization
  DeserializationError err = deserializeJson(doc, inputString);
  if (err)
  {
    //Serial.print(F("deserializeJson() failed with code "));
    //Serial.println(err.c_str());
  }
  //////////////////////////////////////////////////////////////////
  //  JsonObject obj = doc.as<JsonObject>();
  //  Serial.print("the converted string:");
  //  serializeJson(doc, Serial);

  if (!err)
  {
    ///////////////////////////////////////////////////
    JsonObject frame = doc.as<JsonObject>();
    //  serializeJson(doc, Serial);
    //  Serial.println();
    ///////////////////////////////////////////////////
    const char *frameType = doc["H"]; // applying best practice with char pointer
                                      // Serial.print(F("frameType:"));
                                      // Serial.println(frameType);
    if (frameType[0] == 's')
    { /*setupSetting();*/
      if (startReceivingSetting)
      {
        TurnTimersOFF();
        digitalPointer = 0;
        analogPointer = 0; // This when the AVR start sending the setting we have to rest the analog and digital pointers
        startReceivingSetting = false;
        for (char i = 0; i < DnIndex; i++)
        {
          digitalInterval[i] = 0;
          digitalMode[i] = 0;
          digitalTimer[i] = 0;
          pinState[i] = 0;
        }
        for (char i = 0; i < AnIndex; i++)
        {
          analogInterval[i] = 0;
          analogMode[i] = 0;
          analogTimer[i] = 0;
          pinAState[i] = 0;
        }
      }
      //  Serial.println(F("handler sF"));
      for (JsonPair keyValue : frame)
      {
        //    Serial.println(keyValue.key().c_str());
        String item = keyValue.key().c_str();
        //////////////////////////////////////////////////////
        //    Serial.print("substring:");
        //    Serial.println(item.substring(0,1));
        if (item.substring(0, 1) == "D")
        { //Digital pin hadler
          //////////////////////////////////////////////////
          //      Serial.println(F("D handler"));
          char *modePin = doc[item]["m"]; // "o"
          ////////////////////////////////////
          bool newconf = true;
          char oldIndex = 0;
          for (char i = 0; i < digitalPointer; i++)
          {
            if (digitalPin[i] == item.substring(1).toInt())
            {
              newconf = false;
              oldIndex = i;
            }
          }
          //////////////////////////
          if (newconf)
          {
            digitalPin[digitalPointer] = item.substring(1).toInt();
            digitalInterval[digitalPointer] = doc[item]["i"];
            ;
            digitalMode[digitalPointer] = modePin[0];
            digitalPointer++;
            //receivedCommand= true; // incase the user send an command before define the pin as output so by add this line the system will excute the saved states
//            debugUpdate = true; // to print the received setting
          }
          else
          {
            digitalPin[oldIndex] = item.substring(1).toInt();
            digitalInterval[oldIndex] = doc[item]["i"];
            ;
            digitalMode[oldIndex] = modePin[0];
            //receivedCommand= true; // incase the user send an command before define the pin as output so by add this line the system will excute the saved states
//            debugUpdate = true; // to print the updated setting
          }
          ///////////////////////////////////////
        }
        else if (item.substring(0, 1) == "A")
        {                                       //Analog pin hadler
                                                //        Serial.println(F("A handler"));
                                                //        float factor = doc[item]["F"]; // 0.01
                                                //        int interval= doc[item]["i"]; // 5
                                                /////////////////////////////////////////////////
          const char *modePin = doc[item]["m"]; // "o"
          bool newconf = true;
          char oldIndex = 0;
          for (char i = 0; i < analogPointer; i++)
          {
            if (analogPin[i] == (item.substring(1).toInt() + 14))
            {
              newconf = false;
              oldIndex = i;
            }
          }
          //////////////////////////
          if (newconf)
          {
            analogPin[analogPointer] = item.substring(1).toInt() + 14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
            analogInterval[analogPointer] = doc[item]["i"];
            //      analogFactor[analogPointer]=  doc[item]["f"];
            analogMode[analogPointer] = modePin[0];
            analogPointer++;
//            debugUpdate = true; // to print the received setting
          }
          else
          {
            analogPin[oldIndex] = item.substring(1).toInt() + 14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
            analogInterval[oldIndex] = doc[item]["i"];
            //      analogFactor[oldIndex]=  doc[item]["f"];
            analogMode[oldIndex] = modePin[0];
//            debugUpdate = true; // to print the received setting
          }
          //////////////////////////////////
        }
        else if (item.substring(0, 1) == "_")
        {
          Applied = true;
          startReceivingSetting = true;
        }
        // Applied=true;
      }
    }
    else if (frameType[0] == 'c')
    {
      //    Serial.println(F("command frame interpreter initated"));

      ///////////////////////////////////////////
      for (JsonPair keyValue : frame)
      {
        //    Serial.println(keyValue.key().c_str());
        String item = keyValue.key().c_str();
        //////////////////////////////////////////////////////
        //    Serial.print("substring:");
        //    Serial.println(item.substring(0,1));
        if (item.substring(0, 1) == "D")
        { //Digital pin hadler
          //////////////////////////////////////////////////
          //      Serial.println(F("D C handler"));
          //const char* mode = doc[item]["m"]; // "o"
          ////////////////////////////////////
          bool newUpdate = false;
          unsigned char pinNumber = item.substring(1).toInt();
          //for(char i=0;i<digitalPointer;i++)if(pinState[pinNumber]!=doc[item]){newUpdate=true;}
          //////////////////////////
          if (pinState[pinNumber] != doc[item])
          {
            //      Serial.print(F("command change pin:"));
            //      Serial.print(item);
            //      Serial.print(F(" . It will be ="));
            //      Serial.println( pinState[pinNumber]);
            receivedCommand = true; // I moved this switch to execute the chagne only to stop repeated request
            pinState[pinNumber] = doc[item];
          }
          ///////////////////////////////////////////
        }
        else if (item.substring(0, 1) == "A")
        { //Digital pin hadler
          //////////////////////////////////////////////////
          //      Serial.println("A handler");
          //const char* mode = doc[item]["m"]; // "o"
          ////////////////////////////////////
          bool newUpdate = false;
          unsigned char pinNumber = item.substring(1).toInt(); //+14;
                                                               // for(char i=0;i<digitalPointer;i++)if(pinAState[pinNumber]!=doc[item]){newUpdate=true;}
          //////////////////////////
          if (pinAState[pinNumber] != doc[item])
          {
            //      Serial.print(F("command change pinA:"));
            //      Serial.print(item);
            //      Serial.print(F(" . It will be ="));
            //      Serial.println( pinState[pinNumber]);
            receivedCommand = true; // I moved this switch to execute the chagne only to stop repeated request
            pinAState[pinNumber] = doc[item];
          }
          ///////////////////////////////////////////
        }
        else if (item == "in")
        {
          sendInputFrame = true;
          updatePortsStates();
        }
        else if (item == "out")
        {
          sendOutputFrame = true;
        }
      }
    }
    //Serial.println("finsh setup setting part");
  }
}


void settingToNodeMCU()
{

  DynamicJsonDocument doc(100);
  for (unsigned char i = 0; i < digitalPointer; i++)
  {
    String Dn = "D";
    Dn += String(digitalPin[i]);
    doc["H"] = "s";
    JsonObject D_Config = doc.createNestedObject(Dn);
    //JsonArray D_modes = D_Config.createNestedArray("modes");
    ////D_modes.add("a");
    //D_modes.add("i");
    //D_modes.add("o");
    ////D_Config["mode"] =NULL;
    ////D_Config["int"] = NULL;  //// there is no pwd in
    //if(pwmPin[i])D_modes.add("p");
    //JsonObject D_Config = doc.createNestedObject(Dn);
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
    //JsonObject A_Config = doc.createNestedObject(An);
    doc[An]["m"] = String(analogMode[i]);
    doc[An]["i"] = analogInterval[i]; //// there is no pwd in

    serializeJson(doc, Serial);
    Serial.println();
    doc.clear();
    delay(200);
  }

  doc["H"] = "s";
  doc["_applied"] = 1;
  serializeJson(doc, Serial);
  Serial.println();

  //serializeJson(doc, Serial); //this line to for debuging on the same board
  //serializeJson(doc, wifiClient); //this line to send the value over the wifi
}
