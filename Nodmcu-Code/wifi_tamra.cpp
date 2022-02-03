#include"wifi_tamra.h"

////////////////////////////////////
const char *nodeId = "";
String ssid = "";
String password = "";
String ActivationCode = "121233";
String NodeID = "";
unsigned short ROMAddress = 0;
///////////////////////////////////

///////////////////////////////////
WiFiClient espClient;
//PubSubClient client(espClient);
//SoftwareSerial node_arduino(node_rx, node_tx);
//WifiTamra node_setup;
//WiFiServer server(http_port);
//WiFiServer node_server(node_port);
//WiFiClient node_app;
//////////////////////////////////////

//void wifiHotSpot()
//{
//  node_setup.resetSettings();
//  node_setup.autoConnect(wifi_name);
//
//  node_server.begin();
//
//  HTTPClient http; //Declare object of class HTTPClient
//  String postData;
//  ActivationCode = node_setup.getActivationCode();
//  ssid = node_setup.getSsid();
//  password = node_setup.getPassword();
//
//  //http://tamra-nodes.herokuapp.com/
//  String mac = WiFi.macAddress();
//  postData = "{\"mac\":\"" + mac + "\",\"code\":\"" + ActivationCode + "\"}";
//  http.begin("http://tamra-iot.herokuapp.com/node/");    //Specify request destination
//  http.addHeader("Content-Type", "application/json"); //Specify content-type header
//  http.setTimeout(10000);
//  int httpCode = http.POST(postData); //Send the request
//  String payload = http.getString();  //Get the response payload
//  DynamicJsonDocument doc(200);
//  DeserializationError error = deserializeJson(doc, payload);
//  nodeId = doc["_id"];
//  String mystring(nodeId);
//  NodeID = mystring;
//  http.end(); //Close connection
//}
//
//

////////////////////////////
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  //  Serial.println();
  //  Serial.print("Connecting to ");
  //  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //delay(10000);
  while (WiFi.status() != WL_CONNECTED)
  {
    //    WiFi.mode(WIFI_STA);
    //    WiFi.begin(ssid, password);
    // Serial.print(".");
    delay(1000);
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  randomSeed(micros());

  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //  Serial.println("IP address: ");
  //  Serial.println(WiFi.localIP());

}
////////////////////////////
/////////////////////////////////////////////////wifi mangager&activation code&nodeID part
//void debugEEPROM() {
//  Serial.println(F("The save settings in EEPROM"));
//  Serial.print("ssid: ");
//  Serial.println(ssid);
//  Serial.print("password: ");
//  Serial.println(password);
//  Serial.print("ActivationCode: ");
//  Serial.println(ActivationCode);
//  Serial.print("NodeID: ");
//  Serial.println(NodeID);
//}
//void getSavedWifi_nodeID()
//{
//  ROMAddress = 0;
//  ROMAddress++;
//  EEPROM.get(ROMAddress, ssid);
//  ROMAddress += sizeof(ssid);
//  EEPROM.get(ROMAddress, password);
//  ROMAddress += sizeof(password);
//  EEPROM.get(ROMAddress, ActivationCode);
//  ROMAddress += sizeof(ActivationCode);
//  NodeID = "";
//  for (int i = 0; i < NodeIDlength; i++)
//  {
//    char c = EEPROM.read(ROMAddress);
//    NodeID += c; //this is the write flag to use it as indication that the board has a saved configuration
//    ROMAddress++;
//  }
//}
////////////////////////////////////////
//void saveWifi_nodeID()
//{
//
//  ROMAddress = 0;
//  EEPROM.write(ROMAddress, 1); //this is the write flag to use it as indication that the board has a saved configuration
//  EEPROM.commit();
//  ROMAddress++;
//  EEPROM.put(ROMAddress, ssid);
//  EEPROM.commit();
//  ROMAddress += sizeof(ssid);
//  EEPROM.put(ROMAddress, password);
//  EEPROM.commit();
//  ROMAddress += sizeof(password);
//
//  EEPROM.put(ROMAddress, ActivationCode);
//  EEPROM.commit();
//  ROMAddress += sizeof(ActivationCode);
//  for (int i = 0; i < NodeIDlength; i++)
//  {
//    char c = NodeID.charAt(i);
//    EEPROM.write(ROMAddress, c); //this is the write flag to use it as indication that the board has a saved configuration
//    EEPROM.commit();
//    ROMAddress++;
//  }
//}
//
//void GetWifiConfig_OpenHotSpot() {
//  /////////// Getting the saved settings if it saved on AVR
//  EEPROM.begin(700);
//  if (!digitalRead(RestWifi_Button))
//  { //Serial.println("RestWifi_Button is pressed");
//    EEPROM.write(0, 0); //rest saving flag
//    EEPROM.commit();    //this to important must be uncomment in the first time of burning the code over the ic
//  }
//  ////////////////////////////////////////
//  ROMAddress = 0;
//  if (EEPROM.read(ROMAddress))
//  { // if the write flag is high, bring the saved settings and configure them
//    //Serial.println("ROMAdress flag is rised");
//    getSavedWifi_nodeID();
//    //debugEEPROM();
//  }
//  else
//  {
//    wifiHotSpot();
//    saveWifi_nodeID();
//  }
//  ROMAddress = 0;
//}
