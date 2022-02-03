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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
////////////////////////////////////////////
#include <WifiTamra.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
//#include <PubSubClient.h>
#define NodeIDlength 24
#define RestWifi_Button 5 // 5 is D1 see this link => https://microcontrollerslab.com/led-blinking-using-esp8266-nodemcu/

//#define node_tx 1
//#define node_rx 3

#define wifi_name "Nawa Wifi"
#define node_port 5678
#define http_port 80

char buff[] = "Hello Slave\n";

WiFiClient espClient;
//PubSubClient client(espClient);

//SoftwareSerial node_arduino(node_rx, node_tx);
WifiTamra node_setup;
WiFiServer server(http_port);
WiFiServer node_server(node_port);
WiFiClient node_app;

const char* nodeId = "";
String node_fromApp = "";
String node_fromAvr = "";
char data_byte;
char data_byte_arduino;



//const char* ssid = "Apsis 310";
//const char* password = "ilpEdmaN";

String ssid = "";
String password = "";
String ActivationCode="121233";
String NodeID="";


unsigned short ROMAddress=0;
////////////////////////////////////////////
// Update these with values suitable for your network.
//http://www.hivemq.com/demos/websocket-client/
//https://www.hivemq.com/blog/mqtt-client-library-encyclopedia-arduino-pubsubclient/
////////////////////////////////
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
////////////////////////////////
//const char* ssid = "BELL649";
//const char* password = "1ECFE71F23DD";
//const char* ssid = "Apsis 310";
//const char* password = "ilpEdmaN";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
//////////////////////////////////////
const char* mqttServer = "hairdresser-01.cloudmqtt.com";
const int mqttPort = 17358;
const char* mqttUser = "zuatujqk";
const char* mqttPassword = "_jE0VCSHrOAm";
const char* cliendID="Nawa-v1";

//////////////////////////////////////
//WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  5000
char msg[MSG_BUFFER_SIZE];
//#define MQTT_MAX_PACKET_SIZE  2000
int value = 0;
//String clientId{"clientId-T28Dof3bjP"},userName{"abdelrahmanelewah"},passWord{"1234"},outTopic{"hello2"},inTopic{"hello1"};
//clientId="clientId-T28Dof3bjP";
//userName="abdelrahmanelewah";
//passWord="1234";
//inTopic="hello1";
//char* settingsTopic="4321/Nawa-v2/settings";
//char* commandsTopic="4321/Nawa-v2/commands";
//char* outTopic="4321/Nawa-v2/upload";
//char* inputsTopic="4321/Nawa-v2/inputs";
//char* outputsTopic="4321/Nawa-v2/outputs";
//char* stateTopic="4321/Nawa-v2/state";
//////////////////////////////////////////
//String ActivationCode="121233";
//String NodeID="Abcdefghijklmnopqrstvwxyz1234";
///////////////////////////////////
String settingsTopic="/121212/asdssd12323/settings";
String commandsTopic="/121212/asdssd12323/commands";
String outTopic="/121212/asdssd12323/upload";
String inputsTopic="/121212/asdssd12323/inputs";
String outputsTopic="/121212/asdssd12323/outputs";
String stateTopic="/121212/asdssd12323/state";

///////////////////////////////////
#define stateInterval 60
#define Qos  1
unsigned char stateTimer=0;
bool SendingStatetoCLoud=true;
bool SettingFromAVR=false;
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
//issues 1- I write this part on the nodemcu code receivingSettingfromCloud a stackoverflow happend==> solved by adding if no err in the recieved frame
const bool pwmPin[14]={0,0,0,1,0,1,1,0,0,1,1,1,0,0};
///////////////////////////////////////
//#define LastAPin 5
//#define LastDPin 5
///////////////////////////////// intermadiate Varialbe
#define DnIndex 14
#define AnIndex 6
#include <ArduinoJson.h>
//StaticJsonDocument<700> doc; //using staticJson doesn't save the memory chick by compile and see global variables use
//DynamicJsonDocument doc(500);
////////////////////////
unsigned char digitalPointer=0;
unsigned char digitalPin[12]={2,3,4,5,6,7,13}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short  digitalInterval[12]={10,10,10,10,10,10,10,10,10,10};
char digitalMode[12]={'i','i','i','o','o','o'};
unsigned char pinState[14]={1,3,5,7,8,9,4}; // this reserved for the output pin states
unsigned short  digitalTimre[12]={1,66,44,22,11,88,4};
/////////////////////////////////////////////////////////////////
unsigned char  analogPointer=0;
unsigned char  analogPin[6]={1,2,3,4}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short  analogInterval[6]={10,20,30,40,50,60};
unsigned short  analogTimre[6]={20,5,3,9,45};
//float analogFactor[6]={0};
char analogMode[6]={'a','a','a','a','i'};
unsigned short pinAState[6]={20,21,20,20,2120,21}; // this reserved for the output Analog-pin states
////////////////////////////////
//////////////////////////
bool startReceivingSetting=true;
bool receivedSetting=false;
bool receivedCommand=false;
bool debugUpdate=false;
bool Applied=false;
////////////////////////// Swtichs to enable sending Output and Input updates to the cloud
bool SendingOutputUpdate=false;
bool SendingInputUpdate=false;
/////////////////////////////////
unsigned long milless=10000;
bool AVRTalk=false;
bool receivedCloudSettings=false;
bool receivedCloudCommands=false;
/////////////////////////////////
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }

  randomSeed(micros());

//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
inputString="";
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
    inputString+=(char)payload[i];
  }
//  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
if(String(topic) == settingsTopic.c_str()){receivedCloudSettings=true;} 
else if(String(topic) == commandsTopic.c_str()){ receivedCloudCommands=true;}
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
//    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
//   NodeID
    // Attempt to connect
    //if (client.connect(clientId.c_str())) {
      if (client.connect(NodeID.c_str(), mqttUser, mqttPassword, stateTopic.c_str(), Qos, true, "0",true)){ //put your clientId/userName/passWord here      
//    if (client.connect(cliendID, mqttUser, mqttPassword, stateTopic.c_str(), Qos, true, "0",true)){ //put your clientId/userName/passWord here      
//    if (client.connect("clientId-6foAFOeguE","abdsetServerelrahmanelewah","1234")){ //put your clientId/userName/passWord here  
//      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(outTopic, "hello world");
      // ... and resubscribe
      client.subscribe(settingsTopic.c_str(),Qos);
      client.subscribe(commandsTopic.c_str(),Qos);
    } else {
//      Serial.print("failed, rc=");
//      Serial.print(client.state());
//      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  SendingStatetoCLoud=true;
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
   pinMode(RestWifi_Button, INPUT);     // Initialize the BUILTIN_LED pin as an output
//  Serial.begin(115200);
  
  Serial.begin(9600);
 // delay(500);
  
//  while(millis()<milless){
////    while(Serial.available()){
////      char inChar = (char)Serial.read();
////       if(inChar != ' ')inputString += inChar;
////       AVRTalk=true;
////       delay(10);
////       serialEvent();
////      }
//      serialEvent();
//      if(stringComplete){AVRTalk=true;stringComplete=false;}
//      if(AVRTalk){
////      Serial.println(inputString); 
//      AVRsaySomething(); 
//      inputString=""; 
//      stringComplete = false; 
//      milless=millis()+300;
//      AVRTalk=false;
//      //Serial.println("last line in while");
//      }
//      if(Applied)break;
//      }
//  if(!AVRTalk){Applied=true;}
ReceiveSettingFromAVR();

/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// Getting the saved settings if it saved on AVR
  EEPROM.begin(700);
  if(!digitalRead(RestWifi_Button)){
    EEPROM.write(0,0); //rest saving flag
    EEPROM.commit(); //this to important must be uncomment in the first time of burning the code over the ic
//    Serial.println("The rest button is pressed");
    }
  ////////////////////////////////////////

 
 //  NodeID.reserve(50); // old frame test to arroud 10 pins
//  EEPROM.write(0,1); //set saving flag
//  EEPROM.commit(); //this to important must be uncomment in the first time of burning the code over the ic
//  delay(3000);   
   ROMAddress=0;
  if(EEPROM.read(ROMAddress)){  // if the write flag is high, bring the saved settings and configure them
    getSavedWifi_nodeID(); 
  }
  else{
    wifiHotSpot();
    delay(2000);
    saveWifi_nodeID();
  }
 
  ROMAddress=0;
//  Serial.println("after if statement:");
//  Serial.print("EEPROM.read(ROMAddress):");
//  Serial.println(EEPROM.read(ROMAddress));
//  Serial.print("ssid:");
//  Serial.println(ssid);
//  Serial.print("password:");
//  Serial.println(password);
//  Serial.print("ActivationCode:");
//  Serial.println(ActivationCode);
//  Serial.print("NodeID:");
//  Serial.println(NodeID);
//  for(int i=0; i < NodeIDlength; i++){
//    char c = NodeID.charAt(i);
//    Serial.print(c);
//    }
//    Serial.println();
///////////////////////////////////
  createFullTopicDirectories();
    
     
  
  //////////////////////////////////////////////////////////
  setup_wifi();
//  client.setServer(mqtt_server, 1883);
  client.setServer(mqttServer, 17358 );
  client.setCallback(callback);
//////////////////////////////////////////////////
  Serial.print("R"); //sending the Ready Signal to the AVR
  
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

    if (stringComplete) {
     AVRsaySomething();
//    inputString = ""; we stop free up the inputString since some times we will directly reforward the received string from the AVR
    stringComplete = false;
  }
   if(SettingFromAVR){
     ReceiveSettingFromAVR();
     SettingFromAVR=false;
   }
  

    if(Applied){
    sendingSettingtoCloud();
    uploadToCloud(settingsTopic);
    Applied=false;
    }


  if(receivedCloudSettings){
    receivingSettingfromCloud(); // for testing only
    inputString = "";
    stringComplete = false;
    if(receivedCloudSettings){sendingSettingsToAVR();}
    receivedCloudSettings=false;
  } 


  if(receivedCloudCommands){
    sendingCommandsToAVR();
    inputString = "";
    stringComplete = false;
    receivedCloudCommands=false;
    } 

  
  if(SendingOutputUpdate){
//    Serial.println(F("send outputs updates"));
    uploadToCloud(outputsTopic);
    SendingOutputUpdate=false;
    }
  if(SendingInputUpdate){
//    Serial.println(F("send inputs updates"));
    uploadToCloud(inputsTopic);
    SendingInputUpdate=false;
    }       

      if(SendingStatetoCLoud){
    bool retained  =true;   
    //uploadTocloud(stateTopic);
    client.publish(stateTopic.c_str(), "1",retained);
    SendingStatetoCLoud=false;
    } 

  serialEvent();
}

//String settingsTopic="/121212/asdssd12323/settings";
//String commandsTopic="/121212/asdssd12323/commands";
//String inputsTopic="/121212/asdssd12323/inputs";
//String outputsTopic="/121212/asdssd12323/outputs";
//String stateTopic="/121212/asdssd12323/state";

void createFullTopicDirectories(){
  settingsTopic="";
  settingsTopic+=ActivationCode;
  settingsTopic+="/";
  settingsTopic+=NodeID;
  settingsTopic+="/";
  settingsTopic+="settings";
  ////////////////////////////////
  commandsTopic="";
  commandsTopic+=ActivationCode;
  commandsTopic+="/";
  commandsTopic+=NodeID;
  commandsTopic+="/";
  commandsTopic+="commands";
  //////////////////////////////////
  inputsTopic="";
  inputsTopic+=ActivationCode;
  inputsTopic+="/";
  inputsTopic+=NodeID;
  inputsTopic+="/";
  inputsTopic+="inputs";
  /////////////////////////////////
  outputsTopic="";
  outputsTopic+=ActivationCode;
  outputsTopic+="/";
  outputsTopic+=NodeID;
  outputsTopic+="/";
  outputsTopic+="outputs";
  ////////////////////////////////
  stateTopic="";
  stateTopic+=ActivationCode;
  stateTopic+="/";
  stateTopic+=NodeID;
  stateTopic+="/";
  stateTopic+="state";

//  Serial.print("stateTopic: ");
//  Serial.println(stateTopic);

//   Serial.print("settingsTopic: ");
//  Serial.println(settingsTopic);
//
//  Serial.print("commandsTopic: ");
//  Serial.println(commandsTopic);
//
//  Serial.print("inputsTopic: ");
//  Serial.println(inputsTopic);
//
//  Serial.print("outputsTopic: ");
//  Serial.println(outputsTopic);
  
  
  }


void uploadToCloud(String topic){
   bool retained  =true;
   char  openBracket='{';
    //Serial.print("Publish message: ");
    //Serial.println(inputString); char* topic
//    client.publish(outTopic, inputString.c_str() );
//DynamicJsonDocument doc(4000); should work to stop sending two frames in the same message but it doesn't !
//DeserializationError err = deserializeJson(doc, inputString);
//client.publish(topic.c_str(), inputString.c_str(),retained);
if(inputString.indexOf(openBracket,2)==-1 || topic ==  settingsTopic){
  client.publish(topic.c_str(), inputString.c_str(),retained);
  }else{
    catchFrames();
    }
inputString="";
  }

void catchFrames(){
//  Serial.println("multiple frames catched");
  String inputString_Copy=inputString;
  char  closeBracket='}';
  char  openBracket='{';
  bool retained=true;
while(inputString_Copy.length()>1)  {
inputString=inputString_Copy.substring(inputString_Copy.indexOf(openBracket), inputString_Copy.indexOf(closeBracket)+1);
//Serial.print("catched Frame");
//Serial.println(inputString);
DynamicJsonDocument doc(1000); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
///////////////////////////////////////////////////////////// catch any error during serialization 
DeserializationError err = deserializeJson(doc, inputString);
  if (err) {
    inputString_Copy="";
//Serial.print(F("deserializeJson() failed with code "));
//Serial.println(err.c_str());
}
if(!err){ //if  error discard the recieved frame,and if the frame is a complete Json file start handle
 JsonObject frame = doc.as<JsonObject>();
String frameType=doc["H"]; // applying best practice with char pointer
  if(frameType[0]== 'o'){
    uploadToCloud(outputsTopic);
    }
  else if(frameType[0]== 'i'){
    uploadToCloud(inputsTopic);
    }
}
inputString_Copy = inputString_Copy.substring(inputString_Copy.indexOf(closeBracket)+1);
//Serial.print("remain of inputString_Copy");
//Serial.println(inputString_Copy);
} 
  } 

void serialEvent() {
  if(Serial.available()){
    inputString = ""; //here we freeup inputString varabile becuause some frame would send without any need to change or transelate to json 
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if(inChar != ' ')inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    delay(5);
  }
  }
}

///////////////////////////////////////////////////////////////settings handlers
//////////////////////

void sendingSettingtoCloud(){
inputString="";  
DynamicJsonDocument doc(4000);

for(unsigned char i=2; i<DnIndex ;i++){
String Dn="D";  
Dn+=String(i);
JsonObject D_Config = doc.createNestedObject(Dn);
JsonArray D_modes = D_Config.createNestedArray("modes");
//D_modes.add("a");
D_modes.add("i");
D_modes.add("o");
D_Config["mode"] =NULL;  
D_Config["int"] = NULL;  //// there is no pwd in
if(pwmPin[i])D_modes.add("p");
}

for(char i=0;i<digitalPointer;i++){
String Dn="D"; 
Dn+=String(digitalPin[i]);
//JsonObject D_Config = doc.createNestedObject(Dn);
doc[Dn]["mode"]=  String(digitalMode[i]);
doc[Dn]["int"] =  digitalInterval[i];  //// there is no pwd in
  }


for(unsigned char i=0; i<AnIndex ;i++){
String An="A";
An+=String(i);
JsonObject A_Config = doc.createNestedObject(An);
JsonArray A_modes = A_Config.createNestedArray("modes");
A_modes.add("a");
A_modes.add("i");
A_modes.add("o");
A_Config["mode"] =NULL;
A_Config["int"] = NULL;  //// there is no pwd in
}

for(char i=0;i<analogPointer;i++){
String An="A"; 
An+=String(analogPin[i]-14);
//JsonObject A_Config = doc.createNestedObject(An);
doc[An]["mode"]=  String(analogMode[i]);
doc[An]["int"] =  analogInterval[i];  //// there is no pwd in
  }


doc["_applied"] = int(Applied);

//serializeJson(doc, Serial); //this line to for debuging on the same board
//serializeJson(doc, wifiClient); //this line to send the value over the wifi
serializeJson(doc, inputString); 
Serial.println();
}



///////////////////////////////////////
//////////////////////

void receivingSettingfromCloud(){
digitalPointer=0;
analogPointer=0;
DynamicJsonDocument doc(4000);

///////////////////////////////////////////////////////////// catch any error during serialization 
DeserializationError err = deserializeJson(doc, inputString);
  if (err) {
//Serial.print(F("deserializeJson() failed with code "));
//Serial.println(err.c_str());
}
if(!err){
if(!(doc["_applied"])){
///////////////////////////////////////////////////
 JsonObject frame = doc.as<JsonObject>();
//  serializeJson(doc, Serial);
// // serializeJson(doc, wifiClient);
//  Serial.println();
///////////////////////////////////////////////////
//  Serial.println(F("handler sF"));
  for (JsonPair keyValue : frame) {
//    Serial.println(keyValue.key().c_str());
    String item= keyValue.key().c_str();
    //////////////////////////////////////////////////////
//    Serial.print("substring:");
//    Serial.println(item.substring(0,1));
    if(item.substring(0,1)=="D" && (doc[item]["mode"])){//Digital pin hadler
      //////////////////////////////////////////////////
//      Serial.println(F("D handler"));
      String modePin = doc[item]["mode"]; // "o"
      ////////////////////////////////////
      digitalPin[digitalPointer]=item.substring(1).toInt();
      digitalInterval[digitalPointer]= doc[item]["int"];
      digitalMode[digitalPointer]=modePin.charAt(0);
      digitalPointer++;
      }
      else if (item.substring(0,1)=="A" && doc[item]["mode"]!= 0) { //Analog pin hadler
//        Serial.println(F("A handler"));
      String modePin = doc[item]["mode"];
        analogPin[analogPointer]=item.substring(1).toInt()+14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
        analogInterval[analogPointer]=doc[item]["int"];
        analogMode[analogPointer]=modePin.charAt(0);
        analogPointer++;
        //////////////////////////////////
        }else if(item.substring(0,1)=="_"){
          Applied= doc[item];
          }
        receivedCloudSettings=true;
        debugUpdate=true; // to print the received setting  
     }
}
else{receivedCloudSettings=false;}
}
}



//int D3_mode = D3["mode"]; // 0
//int D3_int = D3["int"]; 




///////////////////////////////////////
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
//void serialEvent() {
//  while (Serial.available()) {
//    // get the new byte:
//    char inChar = (char)Serial.read();
//    // add it to the inputString:
//    if(inChar != ' ')inputString += inChar;
//    // if the incoming character is a newline, set a flag so the main loop can
//    // do something about it:
//    if (inChar == '\n') {
//      receivedCloudSettings=true; //for debuging only!!
////      stringComplete = true;
//    }
//  }
//}

///////////////////////////////////////////////////AVR Handler
void AVRsaySomething(){
DynamicJsonDocument doc(1000); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
//DynamicJsonDocument doc(300); // this in case we use dynamic allocation to free the memory, here code accepts around 10 in/out
//DynamicJsonDocument smallDoc(50); 
//StaticJsonDocument<550> doc;
//StaticJsonDocument<50> smallDoc;
///////////////////////////////////////////////////////////// catch any error during serialization 
DeserializationError err = deserializeJson(doc, inputString);
  if (err) {
//Serial.print(F("deserializeJson() failed with code "));
//Serial.println(err.c_str());
}
if(!err){ //if  error discard the recieved frame,and if the frame is a complete Json file start handle
///////////////////////////////////////////////////
 JsonObject frame = doc.as<JsonObject>();
String frameType=doc["H"]; // applying best practice with char pointer
// Serial.print(F("frameType:"));
// Serial.println(frameType);
 if (frameType== "s"){SettingFromAVR=true;}
 else if(frameType== "o"){SendingOutputUpdate=true;}
 else if(frameType== "i"){SendingInputUpdate=true;}
  //Serial.println("finsh setup setting part");
}
}
////////////////////////////////////////////////////////////////
void ReceiveSettingFromAVR(){
 DynamicJsonDocument doc(1000); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
////////////////////////////////
  digitalPointer=0;
  analogPointer=0;   // This when the AVR start sending the setting we have to rest the analog and digital pointers
//  startReceivingSetting=false;
//  for(char i=0;i<DnIndex; i++){
//    digitalInterval[i]= 0;
//    digitalMode[i]=0;
//    }
//  for(char i=0;i<AnIndex; i++){
//    analogInterval[i]= 0;
//    analogMode[i]=0;
//    }
    Applied=false;
    while(!Applied){    
//      StaticJsonDocument<300> doc;
      DeserializationError err = deserializeJson(doc, inputString);
      if (err) {
         Serial.print(F("deserializeJson() failed with code "));
         Serial.println(err.c_str());
      }
      if(!err){ //if  error discard the recieved frame,and if the frame is a complete Json file start handle
         ///////////////////////////////////////////////////
         JsonObject frame = doc.as<JsonObject>();
         String frameType=doc["H"]; // applying best practice with char pointer
// Serial.print(F("frameType:"));
// Serial.println(frameType);
// if (frameType== "s"){ReceiveSettingFromAVR();} 
 

      for (JsonPair keyValue : frame) {
//    Serial.println(keyValue.key().c_str());
      String item= keyValue.key().c_str();
    //////////////////////////////////////////////////////
//    Serial.print("substring:");
//    Serial.println(item.substring(0,1));
    if(item.substring(0,1)=="D"){//Digital pin hadler
      //////////////////////////////////////////////////
//      Serial.println(F("D handler"));
      String modePin = doc[item]["m"]; // "o"
      ////////////////////////////////////
      digitalPin[digitalPointer]=item.substring(1).toInt();
      digitalInterval[digitalPointer]= doc[item]["i"];
      digitalMode[digitalPointer]=modePin.charAt(0);
      digitalPointer++;
      receivedCommand= true; // incase the user send an command before define the pin as output so by add this line the system will excute the saved states
      debugUpdate=true; // to print the received setting
      }
      else if (item.substring(0,1)=="A") { //Analog pin hadler
//        Serial.println(F("A handler"));
//        float factor = doc[item]["F"]; // 0.01
//        int interval= doc[item]["i"]; // 5
/////////////////////////////////////////////////
//      String modePin = doc[item]["m"]; // "o"
      String modePin = doc[item]["m"];
        analogPin[analogPointer]=item.substring(1).toInt()+14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
        analogInterval[analogPointer]=doc[item]["i"];
//      analogFactor[analogPointer]=  doc[item]["f"];
        analogMode[analogPointer]=modePin.charAt(0);
        analogPointer++;
        debugUpdate=true; // to print the received setting  
        //////////////////////////////////
        }else if(item.substring(0,1)=="_"){
          Applied= true;
          startReceivingSetting=true;
          }
        //receivedSetting=true;
     }
       }
       inputString="";
       if(!Applied){stringComplete=false;}
       while(!stringComplete){serialEvent();}
       //Serial.println("last line in while");
      }
}
////////////////////////////////////////////////////////////////
void sendingSettingsToAVR(){
  
DynamicJsonDocument doc(200); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out

for(unsigned char i=0; i<digitalPointer ;i++){
doc["H"] = "s";  
String Dn="D";  
Dn+=String(digitalPin[i]);
doc["H"] = "s";
JsonObject D_Config = doc.createNestedObject(Dn);
//JsonObject D_Config = doc.createNestedObject(Dn);
doc[Dn]["m"]=  String(digitalMode[i]);
doc[Dn]["i"] =  digitalInterval[i];  //// there is no pwd in

serializeJson(doc, Serial);
Serial.println();
doc.clear();
delay(200);
}

for(unsigned char i=0; i<analogPointer ;i++){
doc["H"] = "s";  
String An="A";  
An+=String(analogPin[i]-14);
//doc["H"] = "s";
JsonObject A_Config = doc.createNestedObject(An);
//JsonObject D_Config = doc.createNestedObject(Dn);
doc[An]["m"]=  String(analogMode[i]);
doc[An]["i"] =  analogInterval[i];  //// there is no pwd in

serializeJson(doc, Serial);
Serial.println();
doc.clear();
delay(200);
}

doc["H"] = "s";
doc["_applied"] = int(Applied);
serializeJson(doc, Serial);
inputString="";
//serializeJson(doc, inputString);
Serial.println();
}

/////////////////////////////////////////////////////////

void sendingCommandsToAVR(){
  DynamicJsonDocument doc(400);
  DynamicJsonDocument AVRdoc(400);
  AVRdoc["H"] = "c";
  ///////////////////////////////////////////////////////////// catch any error during serialization 
  DeserializationError err = deserializeJson(doc, inputString);
  if (err) {
//    Serial.print(F("deserializeJson() failed with code "));
//    Serial.println(err.c_str());
    }
  /////////////////////////////////////////////////////////////
  if (!err) {
   JsonObject outdoc = doc["out"].as<JsonObject>();
      for (JsonPair keyValue : outdoc) {
    String key=keyValue.key().c_str();
    //Serial.print(key);
    AVRdoc[key]=doc["out"][key];
    //String value=doc1[item][key];
    //Serial.println(": "+value);
  }
   if(!(doc["in"].isNull())){AVRdoc["in"]=doc["in"];}
   

  serializeJson(AVRdoc, Serial); 
  Serial.println(); 
  }
  }


  /////////////////////////////////////////////////wifi mangager&activation code&nodeID part


  void  getSavedWifi_nodeID(){
//  EEPROM.commit();
  ROMAddress=0;
  ROMAddress++;
  EEPROM.get(ROMAddress, ssid );
  ROMAddress+=sizeof(ssid);
  EEPROM.get(ROMAddress, password );
  ROMAddress+=sizeof(password);
  EEPROM.get(ROMAddress, ActivationCode );
  ROMAddress+=sizeof(ActivationCode);

//  EEPROM.get(ROMAddress, NodeID);
//  ROMAddress+=NodeID.length();
//  Serial.print("NodeID.length(): ");
//  Serial.println(NodeID.length());

  
  NodeID="";
  for(int i=0; i < NodeIDlength; i++){
    char c =EEPROM.read(ROMAddress);
    NodeID+= c; //this is the write flag to use it as indication that the board has a saved configuration
    //EEPROM.commit();
    ROMAddress++;
    }
//  EEPROM.end();
  }
//////////////////////////////////////  
void saveWifi_nodeID(){
  
  ROMAddress=0;
  EEPROM.write(ROMAddress, 1 ); //this is the write flag to use it as indication that the board has a saved configuration
  EEPROM.commit();
  ROMAddress++;
  EEPROM.put(ROMAddress, ssid );
  EEPROM.commit();
  ROMAddress+=sizeof(ssid);
  EEPROM.put(ROMAddress, password );
  EEPROM.commit();
  ROMAddress+=sizeof(password);
  
  EEPROM.put(ROMAddress, ActivationCode );
  EEPROM.commit();
  ROMAddress+=sizeof(ActivationCode);

//  EEPROM.put(ROMAddress, NodeID );
//  EEPROM.commit();
//  ROMAddress+=NodeID.length();
//   NodeID="";
  for(int i=0; i < NodeIDlength; i++){
    char c = NodeID.charAt(i);
    EEPROM.write(ROMAddress, c ); //this is the write flag to use it as indication that the board has a saved configuration
    EEPROM.commit();
    ROMAddress++;
    }
 
//  EEPROM.end();
  }

////////////////////////////////////////////////  

void wifiHotSpot(){
//  node_arduino.begin(9600);
//  pinMode(node_rx, INPUT);
//  pinMode(node_tx, OUTPUT);

//  Serial.begin(115200);
//  Serial.println();
 
  node_setup.resetSettings();
//  Serial.println("Settings reset and now Autoconnecting..");
  node_setup.autoConnect(wifi_name);

//  Serial.print("Connected with Node IP: ");
//  Serial.println(WiFi.localIP());

  node_server.begin();
//  Serial.println("\nServer Set\n");

  HTTPClient http; //Declare object of class HTTPClient
  String postData;



  //postData = "code=" + _code + "&mac=" + mac ;
   ActivationCode = node_setup.getActivationCode();
   ssid = node_setup.getSsid();
   password = node_setup.getPassword();
   
  String mac = WiFi.macAddress();
  postData = "{\"mac\":\"" + mac + "\",\"code\":\"" + ActivationCode + "\"}";
  http.begin("http://tamra-nodes.herokuapp.com/"); //Specify request destination
  http.addHeader("Content-Type", "application/json");         //Specify content-type header

//  Serial.print("Post data : ");
//  Serial.println(postData); //Print HTTP return code

  http.setTimeout(10000);
//  Serial.println("before http.GET();");
  int httpCode = http.POST(postData); //Send the request
  String payload = http.getString();  //Get the response payload
  http.GET();
//  Serial.println("after http.GET();");
//  Serial.println("Http Code : ");
//  Serial.println(httpCode); //Print HTTP return code
//  Serial.println("Payload : ");
//  Serial.println(payload); //Print request response payload
//StaticJsonDocument<200> doc;
DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, payload);

  nodeId = doc["_id"];
  String mystring(nodeId);
  NodeID=mystring;
  http.end(); //Close connection
 
//  Serial.print("Node ID:");
//  Serial.println(nodeId);
//  Serial.print("Node ID string:");
//  Serial.println(NodeID);
  }
