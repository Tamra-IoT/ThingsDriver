/*
  Basic ESP8266 MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.
  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/
#include"tamra_nodemcu.h"

char data_byte;
char data_byte_arduino;
////////////////////////////////////////////
// Update these with values suitable for your network.
//http://www.hivemq.com/demos/websocket-client/
//https://www.hivemq.com/blog/mqtt-client-library-encyclopedia-arduino-pubsubclient/
////////////////////////////////
extern String inputString;     // a String to hold incoming data
extern bool stringComplete; // whether the string is complete
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE 5000
char msg[MSG_BUFFER_SIZE];
//#define MQTT_MAX_PACKET_SIZE  2000
int value = 0;

///////////////////////////////////
extern String settingsTopic; //"/121212/asdssd12323/settings";
extern String commandsTopic;// "/121212/asdssd12323/commands";
extern String outTopic;  //"/121212/asdssd12323/upload";
extern String inputsTopic; //"/121212/asdssd12323/inputs";
extern String outputsTopic; // "/121212/asdssd12323/outputs";
extern String stateTopic;  //"/121212/asdssd12323/state";
///////////////////////////////////
//#define stateInterval 60
//#define Qos 1
unsigned char stateTimer = 0;
bool SendingStatetoCLoud = true;
extern bool SettingFromAVR;
//////////////////////////////////
// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2020
// MIT License
//
// This example shows how to generate a JSON document with ArduinoJson.
//{"H":"s", "D7": {"m": "i", "i":65}}
// https://arduinojson.org/v6/example/generator/
/*setting{"H":"s", "D7": {"m": "i", "i":65}},
  "D8": {"m": "i", "i":70},"D9": {"m": "p", "i":75},"D10": {"m": "p", "i":70}, "D11": {"m": "p", "i":30},"D12": {"m": "o", "i":40}, "D13": {"m": "o", "i":60},"A0": {"m": "o", "i":60}, "A1": {"m": "o", "i":100}, "A2": {"m": "i", "i":100}, "A3": {"m": "i", "i":90}, "A4": {"m": "a", "i":90}, "A5": {"m": "a", "i":100}}
  command {"H":"c", "A3":0, "D13":1, "A2":0, "D12":0, "D10":1}
  /*
  This is the output update frame
  {"H":"o", "D2":255,  "D3":255, "D4":255, "D5":255, "D6":255, "D7":112, "D8":255, "D9":255, "D10":255, "D11":255, "D12":112, "D13":234 ,"A0":255,"A1":255, "A2":255,  "A3":255, "A4":255, "A5":255}
  This is the input update frame
  {"H":"i", "D7":1,  "D8":1, "D9":1, "D10":1, "D11":0, "D12":112, "D5":234}
  {"H":"i", "D2":255,  "D3":255, "D4":255, "D5":255, "D6":255, "D7":112, "D8":255, "D9":255, "D10":255, "D11":255, "D12":112, "D13":234 ,"A0":255,"A1":255, "A2":255,  "A3":255, "A4":255, "A5":255}
  //  received Setting frame from the cloud
  /*
  the commands frame
  {
    "out": { "D2": 0,"D3": 255, "D4": 5, "D5": 25, "D6": 255, "D7": 0, "D8": 1, "D9": 1, "D10": 1 , "D11": 0, "D12": 0, "D13": 1, "A0": 255, "A1": 77, "A2": 31, "A3": 40, "A4": 1, "A4": 0},
    "in": 1
  }
  the settings frame
  {"D2":{"modes":["i","o"],"mode":0,"int":0},"D3":{"modes":["i","o","p"],"mode":0,"int":0},"D4":{"modes":["i","o"],"mode":0,"int":0},"D5":{"modes":["i","o","p"],"mode":0,"int":0},"D6":{"modes":["i","o","p"],"mode":0,"int":0},"D7":{"modes":["i","o"],"mode":"i","int":65},"D8":{"modes":["i","o"],"mode":"i","int":65},"D9":{"modes":["i","o","p"],"mode":"i","int":65},"D10":{"modes":["i","o","p"],"mode":"i","int":65},"D11":{"modes":["i","o","p"],"mode":0,"int":0},"D12":{"modes":["i","o"],"mode":0,"int":0},"D13":{"modes":["i","o"],"mode":0,"int":0},"A0":{"modes":["a","i","o"],"mode":0,"int":0},"A1":{"modes":["a","i","o"],"mode":"a","int":65},"A2":{"modes":["a","i","o"],"mode":"a","int":65},"A3":{"modes":["a","i","o"],"mode":"a","int":65},"A4":{"modes":["a","i","o"],"mode":"i","int":100},"A5":{"modes":["a","i","o"],"mode":"a","int":65},"_applied":1}
*/

extern bool debugUpdate;
bool Applied = false;
////////////////////////// Swtichs to enable sending Output and Input updates to the cloud
extern bool SendingOutputUpdate;
extern bool SendingInputUpdate;
/////////////////////////////////
unsigned long milless = 10000;
bool AVRTalk = false;
extern bool receivedCloudSettings;
bool receivedCloudCommands = false;


void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  ////////////////////////////////////
  ReceiveSettingFromAVR();
  setup_wifi();
  setup_MQTT();
  ////////////////////////////////////
  Serial.print("R"); //sending the Ready Signal to the AVR
}

void loop() {

  checkBrokerConnection();
/////////////////////////////////////////
  if (stringComplete) {
    AVRsaySomething();
    //    inputString = ""; we stop free up the inputString since some times we will directly reforward the received string from the AVR
    stringComplete = false;
  }
/////////////////////////////////////////
  if (SettingFromAVR) {
    ReceiveSettingFromAVR();
    SettingFromAVR = false;
  }
/////////////////////////////////////////
  if (Applied) {
    sendingSettingtoCloud();
    uploadToCloud(settingsTopic);
    Applied = false;
  }
/////////////////////////////////////////
  if (receivedCloudSettings) {
    receivingSettingfromCloud(); // for testing only
    inputString = "";
    stringComplete = false;
    if (receivedCloudSettings) {
      sendingSettingsToAVR();
    }
    receivedCloudSettings = false;
  }
/////////////////////////////////////////
  if (receivedCloudCommands) {
    sendingCommandsToAVR();
    inputString = "";
    stringComplete = false;
    receivedCloudCommands = false;
  }
/////////////////////////////////////////
  if (SendingOutputUpdate) {
    //    Serial.println(F("send outputs updates"));
    uploadToCloud(outputsTopic);
    SendingOutputUpdate = false;
  }
/////////////////////////////////////////
  if (SendingInputUpdate) {
    //    Serial.println(F("send inputs updates"));
    uploadToCloud(inputsTopic);
    SendingInputUpdate = false;
  }
/////////////////////////////////////////
  if (SendingStatetoCLoud) {
    setWellFlag();
    SendingStatetoCLoud = false;
  }
 /////////////////////////////////////////
  serialEvent();
}
