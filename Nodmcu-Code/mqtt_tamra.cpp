#include"mqtt_tamra.h"
//////////////////////////////////////
extern String ActivationCode ;
extern String NodeID ;
extern WiFiClient espClient;
//extern WifiTamra node_setup;
//extern WiFiServer server(http_port);
//extern WiFiServer node_server(node_port);
//extern WiFiClient node_app;
////////////////////////
extern String inputString ;
extern bool receivedCloudSettings;
extern bool receivedCloudCommands;
extern bool SendingStatetoCLoud;
//////////////////////////////////////
PubSubClient client(espClient);
///////////////////////////////
//const char *mqttServer = "hairdresser-01.cloudmqtt.com";
const char* mqtt_server = "broker.mqtt-dashboard.com";
const int mqttPort = 1883;
const char *mqttUser = "zuatujqk";
const char *mqttPassword = "_jE0VCSHrOAm";
const char *cliendID = "Nawa-v1";
///////////////////////////////////

String settingsTopic="settings";
String commandsTopic="commands";
String outTopic="upload";
String inputsTopic="inputs";
String outputsTopic="outputs";
String stateTopic="state";


void catchFrames()
{
  //  Serial.println("multiple frames catched");
  String inputString_Copy = inputString;
  char closeBracket = '}';
  char openBracket = '{';
  bool retained = true;
  while (inputString_Copy.length() > 1)
  {
    inputString = inputString_Copy.substring(inputString_Copy.indexOf(openBracket), inputString_Copy.indexOf(closeBracket) + 1);
    DynamicJsonDocument doc(1000); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
    ///////////////////////////////////////////////////////////// catch any error during serialization
    DeserializationError err = deserializeJson(doc, inputString);
    if (err)
    {
      inputString_Copy = "";
    }
    if (!err)
    { //if  error discard the recieved frame,and if the frame is a complete Json file start handle
      JsonObject frame = doc.as<JsonObject>();
      String frameType = doc["H"]; // applying best practice with char pointer
      if (frameType[0] == 'o')
      {
        uploadToCloud(outputsTopic);
      }
      else if (frameType[0] == 'i')
      {
        uploadToCloud(inputsTopic);
      }
    }
    inputString_Copy = inputString_Copy.substring(inputString_Copy.indexOf(closeBracket) + 1);
  }
}


//////////////////////////////////////////

void uploadToCloud(String topic)
{
  bool retained = true;
  char openBracket = '{';
  if (inputString.indexOf(openBracket, 2) == -1 || topic == settingsTopic)
  {
    client.publish(topic.c_str(), inputString.c_str(), retained);
  }
  else
  {
    catchFrames();
  }
  inputString = "";
}

//////////////////////////////////////////




///////////////////

void callback(char *topic, byte *payload, unsigned int length)
{
  inputString = "";
  //  Serial.print("Message arrived [");
  //  Serial.print(topic);
  //  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    //    Serial.print((char)payload[i]);
    inputString += (char)payload[i];
  }
  //  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
  }
  if (String(topic) == settingsTopic.c_str())
  {
    receivedCloudSettings = true;
  }
  else if (String(topic) == commandsTopic.c_str())
  {
    receivedCloudCommands = true;
  }
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
//      if (client.connect(NodeID.c_str(), mqttUser, mqttPassword, stateTopic.c_str(), Qos, true, "0",true)){ //put your clientId/userName/passWord here      
//    if (client.connect(cliendID, mqttUser, mqttPassword, stateTopic.c_str(), Qos, true, "0",true)){ //put your clientId/userName/passWord here      
    if (client.connect("clientId-6foAFOeguE","abdelrahmanelewah","1234",stateTopic.c_str(), Qos, true, "0")){ //put your clientId/userName/passWord here      
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
////////////////////////

void setWellFlag() {
  bool retained = true;
  client.publish(stateTopic.c_str(), "1", retained);
}

void checkBrokerConnection() {
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
////////////////////////
//////////////
void setup_MQTT(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
////////////////////////
