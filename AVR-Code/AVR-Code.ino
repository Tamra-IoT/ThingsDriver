/*
1-Deal with big JSON --> deserialize the comming JSON--->interpret setting and command frames-> execute setting and command frames
2-Remove factor parmmters
3- adding Second timer
Hints:
i- Naming of pinState and pinAstate is complicated to made a relation between the pin number and it's oldstate 
to check if there is an update or it repeated command. I'm not sure if there is more simple way or no =D 
*/
/////////////EEPROM configurations

#include <EEPROM.h>
#define DnIndex 14
#define AnIndex 6
unsigned short ROMAddress=0;
//EEPROM.update(0,0); // this command make a factory rest for the board

////////////////////////////////

#include <ArduinoJson.h>
//StaticJsonDocument<700> doc; //using staticJson doesn't save the memory chick by compile and see global variables use
//DynamicJsonDocument doc(500);
////////////////////////
char digitalPointer=0;
unsigned char digitalPin[DnIndex]={'0'}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short  digitalInterval[DnIndex]={0};
char digitalMode[DnIndex]={'0'};
unsigned char pinState[DnIndex]={0}; // this reserved for the output pin states
unsigned short  digitalTimer[DnIndex]={0,0,0,0,0,0,0,0,0,0,0,0};
///////////////////////////////////////////////////////////////
char  analogPointer=0;
unsigned char  analogPin[AnIndex]={'0'}; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
unsigned short  analogInterval[AnIndex]={0};
unsigned short  analogTimer[AnIndex]={0,0,0,0,0,0};
//float analogFactor[AnIndex]={0};
char analogMode[AnIndex]={'0'};
unsigned short pinAState[AnIndex]={0.0,0.0,0.0,0.0,0.0,0.0}; // this reserved for the output Analog-pin states
///////////////////////////// variable to handle nodeMCU communication
const bool pwmPin[14]={0,0,0,1,0,1,1,0,0,1,1,1,0,0};
////////////////////////
//timer variables
bool tick= false;
bool startReceivingSetting=true;
////////////////////////////////////////////////////////////////////////////////////////////////////
void initialize_timer()
{
 cli();          // disable global interrupts
  
  // initialize Timer1 for interrupt @ 1000 msec
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  
  sei();          // enable global interrupts
}

////////////////////////////////////////////////////////////////////////////////////////////////////



// timer interrupt routine
ISR(TIMER1_COMPA_vect)
{ 
  //sensortimer++;  
  tick=true;
}
  
  
////////////////////////////////////////////////////////////////////////////////////////////////////






//////////////////////////
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


//full setting frame 
/*{"H":"s", "D2": {"m": "i", "i":30},"D3": {"m": "o", "i":45},"D4": {"m": "i", "i":50},"D5": {"m": "o", "i":55},"D6": {"m": "i", "i":60},"D7": {"m": "i", "i":65},
"D8": {"m": "i", "i":70},"D9": {"m": "p", "i":75},"D10": {"m": "p", "i":70}, "D11": {"m": "p", "i":30},"D12": {"m": "o", "i":40}, "D13": {"m": "o", "i":60},"A0": {"m": "o", "i":60}, "A1": {"m": "o", "i":100}, "A2": {"m": "i", "i":100}, "A3": {"m": "i", "i":90}, "A4": {"m": "a", "i":90}, "A5": {"m": "a", "i":100}}
*/
// The new setting frame after dividing the settings
/* 
{"H":"s","D13":{"m":"o","i":13}}
{"H":"s","D12":{"m":"i","i":12}}
{"H":"s","D11":{"m":"p","i":11}}
{"H":"s","A5":{"m":"a","i":15}}
{"H":"s","A4":{"m":"i","i":14}}
{"H":"s","A3":{"m":"o","i":13}}
{"H":"s","_applied":1}
//the commands 
{"H":"c","D13":1}
{"H":"c","D2":0,"D3":255,"D4":5,"D5":25,"D6":255,"D7":0,"D8":1,"D9":1,"D10":1,"D11":0,"D12":0,"D13":1,"A0":255,"A1":77,"A2":31,"A3":40,"A4":0,"in":1}
*/
////////////////
bool Applied=false;
bool receivedCommand=false;
bool debugUpdate=false;
bool sendOutputFrame=false;
bool sendInputFrame=false;
////////////////////////////////

//https://cpp4arduino.com/2018/11/06/what-is-heap-fragmentation.html?fbclid=IwAR2QCA6w5v_f9PBGwZAwPqQ6rOacEGDmIJtfDyd_PiP-93Pc0UGjON8mBRM
//https://arduinojson.org/v6/api/jsondocument/#:~:text=StaticJsonDocument%20vs%20DynamicJsonDocument&text=Use%20a%20StaticJsonDocument%20to%20store,for%20documents%20larger%20than%201KB)
//https://learn.adafruit.com/memories-of-an-arduino/optimizing-sram
// using global dynamic json object or locally still and open issue

void setup() {
//   EEPROM.write(0,0); //this to important must be uncomment in the first time of burning the code over the ic

  // initialize serial:
  Serial.begin(9600);
//  Serial.begin(115200);
  //DynamicJsonDocument doc(300);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200); // old frame test to arroud 10 pins
  delay(1500); //take time until the power become stables  
//  inputString.reserve(420); // the new frame for 13 pins
//  Serial.println(A0-1); //check the value of A0; it equals to 14 so A1 15 and so on the the An equal to the received value + 14
////////////////////////////////Epprom parts
if(EEPROM.read(ROMAddress)){  // if the write flag is high, bring the saved settings and configure them
//  Serial.println("Getting the saved settings");
  getSavedSettings(); // printSavedSettings();
  digitalSetup();analogSetup();
  settingToNodeMCU();
  updatePortsStates();
  }
//else{updateSettings();        Serial.println("update called");} for debuging purpose
//////////////////////////////////
 //delay(30000); //wait to estiblish the wifi connection and mqtt server 
 do{}while(!Serial.available() || Serial.read() != 'R');
// Serial.println("Received Ready signal correctly");
/////////////////////////////
initialize_timer();    
/////////////////////////////////
sendOutputFrame=true; // sending the outframe in the after inialze the board// the app bring older states
}
////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {

  // print the string when a newline arrives:
  if (stringComplete) {
//    Serial.println(inputString);
    // clear the string:
//    Serial.println(inputString.length());
    jsonHandler();
    inputString = "";
    stringComplete = false;
  }
  
  if(Applied){
    digitalSetup();
    analogSetup();
    updateSettings();
    settingToNodeMCU();
    delay(1000);
    sendUFrame('o');
    TurnTimersON();
    Applied=false;
    }

      if(receivedCommand){
    updatePortsStates();
    updateSettings();
    sendUFrame('o');
    receivedCommand=false;
    }

     if(sendOutputFrame){
      sendUFrame('o');
      sendOutputFrame=false;
      }
     if(sendInputFrame){
      sendUFrame('i');
      sendInputFrame=false;
      }  

//    if(debugUpdate){
//      printDebugInfo();
//      debugUpdate=false;
//      }

    if(tick){
      //Serial.println(F("tick"));
      for(char i =0;i<digitalPointer;i++){++digitalTimer[i];}
      for(char i =0;i<analogPointer; i++){++analogTimer[i];}  
      tick=false;
      }
    checkAlarms();   
}
////////////////////////
void TurnTimersON(){
TIMSK1 |= (1 << OCIE1A); //enable timer1
  }
  
void TurnTimersOFF(){
 TIMSK1 &= ~(1 << OCIE1A); // turn off the timer interrupt
  }

////////////////////////////////////////////
void checkAlarms(){
  for(unsigned char i =0;i<digitalPointer;i++){
    if(digitalTimer[i]>= digitalInterval[i]){
      //sendDUpdate(i);  //archieved due to the cloud integration
      updatePortsStates(); // this function should rename because it's get and update all the states
     if(digitalMode[i]=='i'){ sendUFrame(digitalMode[i]);}
      digitalTimer[i]=0;
      }
    }
  for(unsigned char i =0;i<analogPointer; i++){
    if(analogTimer[i]>= analogInterval[i]){
//      sendAUpdate(i); //archieved due to the cloud integration
      updatePortsStates(); // this function should rename because it's get and update all the states
      if(analogMode[i]=='i' || analogMode[i]=='a') {sendUFrame(analogMode[i]);}
      analogTimer[i]=0;
      }
    }   
  }

void sendUFrame( char mode){
  unsigned char modes[2];
  if(mode == 'o' || mode == 'p'){ modes[0]='o'; modes[1]='p'; mode='o';}
  else if(mode == 'i' || mode == 'a'){ modes[0]='i'; modes[1]='a'; mode='i';} // here, we aggregate the modes that are defined on the cloud as input or outputs
  DynamicJsonDocument doc(250); 
  doc["H"] = String(mode);
//Serial.println(sizeof(modes));
for(unsigned char j=0 ; j< sizeof(modes);j++){
  
  for(unsigned char i =0;i<digitalPointer;i++){
    if(modes[j]==digitalMode[i]){
    String Dn="D";
    Dn+=String(digitalPin[i]);
    //if(modes[j] == digitalMode[i]){doc[Dn] = pinState[digitalPin[i]];} // two fitler the by mode
    doc[Dn] = pinState[digitalPin[i]];
    digitalTimer[i]=0;   // these to work in the shortest time interval mode for each mode
    if(digitalTimer[i]>= digitalInterval[i]){digitalTimer[i]=0;} // these to  sending only one frame for two ports or more that have the same time interval 
    }
  }
  for(unsigned char i =0;i<analogPointer; i++){
    unsigned char pinNumber=analogPin[i]-14;
    if(modes[j] == analogMode[i]){
    String An="A";
    An+=String(pinNumber);
    doc[An] = pinAState[pinNumber];
    analogTimer[i]=0;   // these to work in the shortest time interval mode for each mode
    //if(analogTimer[i]>= analogInterval[i]){analogTimer[i]=0;} // these to  sending only one frame for two ports or more that have the same time interval 
    }
  }
  }
  serializeJson(doc, Serial);
  Serial.println();
 }  


void updatePortsStates(){
//  Serial.println(F("updatePortsStates called successfully"));
  for(char i =0;i<digitalPointer; i++){
     switch(digitalMode[i]){
      case 'i': pinState[digitalPin[i]]=digitalRead(digitalPin[i]);// Serial.print(F("pin:D"));Serial.print(digitalPin[i]); Serial.print(F("become=")); Serial.println( pinState[digitalPin[i]]); 
      break;
      case 'o': digitalWrite(digitalPin[i], pinState[digitalPin[i]]);//Serial.print(F("pin:D"));Serial.print(digitalPin[i]); Serial.print(F("become=")); Serial.println( pinState[digitalPin[i]]); 
      break;
      case 'p': analogWrite(digitalPin[i], pinState[digitalPin[i]]);//Serial.print(F("pinPWM:"));Serial.print(digitalPin[i]); Serial.print(F("become=")); ;Serial.println( pinState[digitalPin[i]]); 
      break;
     }}
     /////////////////////////

  for(char i =0;i<analogPointer; i++){
    unsigned char pinNumber=analogPin[i]-14;
     switch(analogMode[i]){
    //case 'o': digitalWrite(analogPin[i], pinAState[(analogPin[i]-14)]);Serial.print(F("pin:A"));Serial.print((analogPin[i]-14)); Serial.print(F("become=")); Serial.println( pinAState[analogPin[i]-14]); 
      case 'o': digitalWrite(analogPin[i], pinAState[pinNumber]);//Serial.print(F("pin:A"));Serial.print(pinNumber); Serial.print(F("become=")); Serial.println( pinAState[pinNumber]); 
      break;
      case 'i': pinAState[pinNumber]=digitalRead(analogPin[i]);// Serial.print(F("pin:A"));Serial.print(pinNumber); Serial.print(F("become=")); Serial.println( pinAState[pinNumber]); 
      break;
      case 'a': pinAState[pinNumber]=analogRead(analogPin[i]);// Serial.print(F("pin:A"));Serial.print(pinNumber); Serial.print(F("become=")); Serial.println( pinAState[pinNumber]); 
      break;
//      case 'p': analogWrite(analogPin[i], pinState[digitalPin[i]]);Serial.print(F("pinPWM:"));Serial.print(digitalPin[i]-14); Serial.print(F("become=")); ;Serial.println( pinState[digitalPin[i]]); 
//      break;  // no pwm over Analog pin on the atmega328pa
     }}

     
     
  }

void digitalSetup(){
  char i=0;
//  Serial.println(F("from digital Handler"));
//  while(digitalPin[i]!='0'){
  for(i=0; i<digitalPointer; i++)  {
    switch(digitalMode[i]){
      case 'i': pinMode(digitalPin[i], INPUT);//Serial.print(F("pinIn:"));  Serial.println(digitalPin[i]); 
      break;
      case 'o': pinMode(digitalPin[i], OUTPUT);//digitalWrite(digitalPin[i], LOW);//pinState[digitalPin[i]]=0;//Serial.print(F("pinOut:"));Serial.println(digitalPin[i]); 
      break;
      case 'p': pinMode(digitalPin[i], OUTPUT);//Serial.print(F("pinPWD:"));Serial.println(digitalPin[i]); 
      break;
      default:;//Serial.println(F("undefined mode insde digital handlers"));   
      }

    }
    }
    
void analogSetup(){
  char i=0;
//  Serial.println(F("from analog Handler"));
  //while(analogPin[i]!='0'){
  for(i=0; i<analogPointer; i++)  {
    /////////////////////////////////////////////////
    switch(analogMode[i]){
      case 'i': pinMode(analogPin[i], INPUT);//Serial.print(F("pinAIn:"));  Serial.println(digitalPin[i]); 
      break;
      case 'o': pinMode(analogPin[i], OUTPUT); //digitalWrite(analogPin[i], LOW); pinAState[analogPin[i]-14]=0;//Serial.print(F("pinAOut:"));Serial.println(analogPin[i]); 
      break;
      case 'a': pinMode(analogPin[i], INPUT);//Serial.print(F("pinA:"));Serial.println(analogPin[i]); 
      break;
      default:;//Serial.println(F("undefined mode insde analog handlers"));   
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

void jsonHandler(){
DynamicJsonDocument doc(500); // this in case we use dynamic allocation to free the memory, here code accepts around 13 in/out
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
//////////////////////////////////////////////////////////////////
//  JsonObject obj = doc.as<JsonObject>();
//  Serial.print("the converted string:");
//  serializeJson(doc, Serial);

if(!err){
///////////////////////////////////////////////////
 JsonObject frame = doc.as<JsonObject>();
//  serializeJson(doc, Serial);
//  Serial.println();
///////////////////////////////////////////////////
const char* frameType=doc["H"]; // applying best practice with char pointer
// Serial.print(F("frameType:"));
// Serial.println(frameType);
 if (frameType[0]== 's'){/*setupSetting();*/
  if(startReceivingSetting){
    TurnTimersOFF();
    digitalPointer=0;
    analogPointer=0;   // This when the AVR start sending the setting we have to rest the analog and digital pointers
    startReceivingSetting=false;
    for(char i=0;i<DnIndex; i++){
      digitalInterval[i]= 0;
      digitalMode[i]=0;
      digitalTimer[i]=0;
      pinState[i]=0;
      }
    for(char i=0;i<AnIndex; i++){
      analogInterval[i]= 0;
      analogMode[i]=0;
      analogTimer[i]=0;
      pinAState[i]=0;
      }  
     }
//  Serial.println(F("handler sF"));
  for (JsonPair keyValue : frame) {
//    Serial.println(keyValue.key().c_str());
    String item= keyValue.key().c_str();
    //////////////////////////////////////////////////////
//    Serial.print("substring:");
//    Serial.println(item.substring(0,1));
    if(item.substring(0,1)=="D"){//Digital pin hadler
      //////////////////////////////////////////////////
//      Serial.println(F("D handler"));
      char* modePin = doc[item]["m"]; // "o"
      ////////////////////////////////////
      bool newconf=true;
      char oldIndex=0;
      for(char i=0;i<digitalPointer;i++){if(digitalPin[i]== item.substring(1).toInt()){newconf=false;  oldIndex=i;}}
      //////////////////////////
      if(newconf){
      digitalPin[digitalPointer]=item.substring(1).toInt();
      digitalInterval[digitalPointer]= doc[item]["i"];;
      digitalMode[digitalPointer]=modePin[0];
      digitalPointer++;
      //receivedCommand= true; // incase the user send an command before define the pin as output so by add this line the system will excute the saved states
      debugUpdate=true; // to print the received setting
      }
      else{
      digitalPin[oldIndex]=item.substring(1).toInt();
      digitalInterval[oldIndex]= doc[item]["i"];;
      digitalMode[oldIndex]=modePin[0];
      //receivedCommand= true; // incase the user send an command before define the pin as output so by add this line the system will excute the saved states
      debugUpdate=true; // to print the updated setting  
        }
      ///////////////////////////////////////
      }
      else if (item.substring(0,1)=="A") { //Analog pin hadler
//        Serial.println(F("A handler"));
//        float factor = doc[item]["F"]; // 0.01
//        int interval= doc[item]["i"]; // 5
/////////////////////////////////////////////////
      const char* modePin = doc[item]["m"]; // "o"
      bool newconf=true;
      char oldIndex=0;
      for(char i=0;i<analogPointer;i++){if(analogPin[i]== (item.substring(1).toInt()+14)){newconf=false;  oldIndex=i;}}
      //////////////////////////
      if(newconf){
        analogPin[analogPointer]=item.substring(1).toInt()+14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
        analogInterval[analogPointer]=doc[item]["i"];
//      analogFactor[analogPointer]=  doc[item]["f"];
        analogMode[analogPointer]=modePin[0]; 
        analogPointer++;
        debugUpdate=true; // to print the received setting  
      }
      else{
        analogPin[oldIndex]=item.substring(1).toInt()+14; // the max posible value is 12 and may reduce and set 0 to know the real length by checking 0 ascii value
        analogInterval[oldIndex]=doc[item]["i"];
//      analogFactor[oldIndex]=  doc[item]["f"];
        analogMode[oldIndex]=modePin[0]; 
        debugUpdate=true; // to print the received setting  
           }
        //////////////////////////////////
        }else if(item.substring(0,1)=="_"){
          Applied= true;
          startReceivingSetting=true;
          }
       // Applied=true;
     }
  }else if(frameType[0]== 'c'){
//    Serial.println(F("command frame interpreter initated"));

    ///////////////////////////////////////////
      for (JsonPair keyValue : frame) {
//    Serial.println(keyValue.key().c_str());
    String item= keyValue.key().c_str();
    //////////////////////////////////////////////////////
//    Serial.print("substring:");
//    Serial.println(item.substring(0,1));
    if(item.substring(0,1)=="D"){//Digital pin hadler
      //////////////////////////////////////////////////
//      Serial.println(F("D C handler"));
      //const char* mode = doc[item]["m"]; // "o"
      ////////////////////////////////////
      bool newUpdate=false;
      unsigned char pinNumber=item.substring(1).toInt();
      //for(char i=0;i<digitalPointer;i++)if(pinState[pinNumber]!=doc[item]){newUpdate=true;}
      //////////////////////////
      if(pinState[pinNumber]!=doc[item]){
//      Serial.print(F("command change pin:"));
//      Serial.print(item);
//      Serial.print(F(" . It will be ="));
//      Serial.println( pinState[pinNumber]);
      receivedCommand=true; // I moved this switch to execute the chagne only to stop repeated request
      pinState[pinNumber]=doc[item];
     }
    ///////////////////////////////////////////
    }
    else if(item.substring(0,1)=="A"){//Digital pin hadler
      //////////////////////////////////////////////////
//      Serial.println("A handler");
      //const char* mode = doc[item]["m"]; // "o"
      ////////////////////////////////////
      bool newUpdate=false;
      unsigned char pinNumber=item.substring(1).toInt();//+14;
     // for(char i=0;i<digitalPointer;i++)if(pinAState[pinNumber]!=doc[item]){newUpdate=true;}
      ////////////////////////// 
      if(pinAState[pinNumber]!=doc[item]){
//      Serial.print(F("command change pinA:"));
//      Serial.print(item);
//      Serial.print(F(" . It will be ="));
//      Serial.println( pinState[pinNumber]);
      receivedCommand=true; // I moved this switch to execute the chagne only to stop repeated request
      pinAState[pinNumber]=doc[item];
      }
    ///////////////////////////////////////////
    }else if(item=="in"){sendInputFrame=true;}
     else if(item=="out"){sendOutputFrame=true;}
    }
        
    }
  //Serial.println("finsh setup setting part");
 
}
}
/////////////////////////////////
// the recommendations here for the setting frame
//const char* D9_m = doc["D9"]["m"]; // "o"
//int D9_i = doc["D9"]["i"]; // 100
//
//float A1_F = doc["A1"]["F"]; // 0.01
//int A1_i = doc["A1"]["i"]; // 5
/////////////////////////////////
//void setupSetting(){Serial.println("the handler call setup setting function");}
///////////////////////////////////
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if(inChar != ' ')inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    delay(5); // comment this line if you chose 9600 baudrade in case of using 115200 this should included
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}



/////////////////////// EEPROM functions

void getSavedSettings(){
  ROMAddress=0;
  ROMAddress++;
  EEPROM.get(ROMAddress, digitalPointer );
  ROMAddress+=sizeof(digitalPointer);
  EEPROM.get(ROMAddress, analogPointer );
  ROMAddress+=sizeof(analogPointer);
  
  for(unsigned char i=0; i<DnIndex ; i++){EEPROM.get(ROMAddress, pinState[i] ); ROMAddress+=sizeof(pinState[i]);}
  for(unsigned char i=0; i<AnIndex ; i++){EEPROM.get(ROMAddress, pinAState[i] ); ROMAddress+=sizeof(pinAState[i]);}
  
  for(char i=0; i<digitalPointer;i++){
    EEPROM.get(ROMAddress, digitalPin[i] ); ROMAddress+=sizeof(digitalPin[i]);
    EEPROM.get(ROMAddress, digitalInterval[i] ); ROMAddress+=sizeof(digitalInterval[i]);
    EEPROM.get(ROMAddress, digitalMode[i] ); ROMAddress+=sizeof(digitalMode[i]);
//    EEPROM.get(ROMAddress, digitalTimer[i] ); ROMAddress+=sizeof(digitalTimer[i]);
    }

      for(char i=0; i<analogPointer;i++){
    EEPROM.get(ROMAddress, analogPin[i] ); ROMAddress+=sizeof(analogPin[i]);
    EEPROM.get(ROMAddress, analogInterval[i] ); ROMAddress+=sizeof(analogInterval[i]);
    EEPROM.get(ROMAddress, analogMode[i] ); ROMAddress+=sizeof(analogMode[i]);
//    EEPROM.get(ROMAddress, analogTimer[i] ); ROMAddress+=sizeof(analogTimer[i]);
    }
    
 
  }

  void updateSettings(){
//  for(char i; i <AnIndex; i++ ){pinAState[i]=0;}
  ROMAddress=0;
  EEPROM.update(ROMAddress, 1 ); //this is the write flag to use it as indication that the board has a saved configuration
  ROMAddress++;
  EEPROM.update(ROMAddress, digitalPointer );
  ROMAddress+=sizeof(digitalPointer);
  EEPROM.update(ROMAddress, analogPointer );
  ROMAddress+=sizeof(analogPointer);
  
  for(char i=0; i<DnIndex ; i++){EEPROM.update(ROMAddress, pinState[i] ); ROMAddress+=sizeof(pinState[i]);}
  for(char i=0; i<AnIndex ; i++){EEPROM.update(ROMAddress, pinAState[i] ); ROMAddress+=sizeof(pinAState[i]);}

  
  for(char i=0; i<digitalPointer;i++){
    EEPROM.update(ROMAddress, digitalPin[i] ); ROMAddress+=sizeof(digitalPin[i]);
    EEPROM.update(ROMAddress, digitalInterval[i] ); ROMAddress+=sizeof(digitalInterval[i]);
    EEPROM.update(ROMAddress, digitalMode[i] ); ROMAddress+=sizeof(digitalMode[i]);
//    EEPROM.update(ROMAddress, digitalTimer[i] ); ROMAddress+=sizeof(digitalTimer[i]);
    }
    
  


    for(char i=0; i<analogPointer;i++){
    EEPROM.update(ROMAddress, analogPin[i] ); ROMAddress+=sizeof(analogPin[i]);
    EEPROM.update(ROMAddress, analogInterval[i] ); ROMAddress+=sizeof(analogInterval[i]);
    EEPROM.update(ROMAddress, analogMode[i] ); ROMAddress+=sizeof(analogMode[i]);
//    EEPROM.update(ROMAddress, analogTimer[i] ); ROMAddress+=sizeof(analogTimer[i]);
    }
    
  

    

  
  }  




//////////////////////////////////////AVR==settings==NodeMCU
void settingToNodeMCU(){

  DynamicJsonDocument doc(100);
for(unsigned char i=0; i<digitalPointer ;i++){
String Dn="D";  
Dn+=String(digitalPin[i]);
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
doc[Dn]["m"]=  String(digitalMode[i]);
doc[Dn]["i"] =  digitalInterval[i];  //// there is no pwd in

serializeJson(doc, Serial);
Serial.println();
doc.clear();
delay(200);
}



for(unsigned char i=0;i<analogPointer;i++){

doc["H"] = "s";
String An="A"; 
An+=String(analogPin[i]-14);
//JsonObject A_Config = doc.createNestedObject(An);
doc[An]["m"]=  String(analogMode[i]);
doc[An]["i"] =  analogInterval[i];  //// there is no pwd in

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

/////////////////////////////////////////
///////////////////////////////////////////
//Printing the settings after each new commands in the debuging mode
/*
void printDebugInfo(){
  for(char i =0 ; i<digitalPointer;i++){
    Serial.print(F("pin,mode,interval,timer:D"));
    Serial.print(digitalPin[i]);
    Serial.print(F(","));
    Serial.print( digitalMode[i]);
    Serial.print(F(","));
    Serial.print( digitalInterval[i]);
    Serial.print(F(","));
    Serial.println( digitalTimer[i]);
    }
  for(char i=0; i<analogPointer; i++)  {
    Serial.print(F("pin,mode,interval:A"));
    Serial.print((analogPin[i]-14));
    Serial.print(F(","));
    Serial.print(analogMode[i]);
//  Serial.print(analogFactor[i],4);
    Serial.print(F(","));
    Serial.print(analogInterval[i]);
    Serial.print(F(","));
    Serial.println( analogTimer[i]);
//  Serial.print(F(","));

    } 
  }
*/ 


////////////////////////////////////////////////////////////////

//Printing the saved settings after retrive from the EPPROM
/*  
void printSavedSettings(){
  
  Serial.print(F("digitalPointer="));
  Serial.println(digitalPointer);
  Serial.print(F("analogPointer="));
  Serial.println(analogPointer);
  for(char i=0; i<digitalPointer;i++){Serial.print(F("digitalPin["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(digitalPin[i]);}
  for(char i=0; i<digitalPointer;i++){Serial.print(F("digitalInterval["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(digitalInterval[i]);}
  for(char i=0; i<digitalPointer;i++){Serial.print(F("digitalMode["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(digitalMode[i]);}
  for(char i=0; i<digitalPointer;i++){Serial.print(F("digitalTimer["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(digitalTimer[i]);}
  for(char i=0; i<DnIndex ; i++){Serial.print(F("pinState["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(pinState[i]);}

  for(char i=0; i<analogPointer;i++){Serial.print(F("analogPin["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(analogPin[i]);}
  for(char i=0; i<analogPointer;i++){Serial.print(F("analogInterval["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(analogInterval[i]);}
  for(char i=0; i<analogPointer;i++){Serial.print(F("analogMode["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(analogMode[i]);}
  for(char i=0; i<analogPointer;i++){Serial.print(F("analogTimer["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(analogTimer[i]);}
  for(char i=0; i<AnIndex ; i++){Serial.print(F("pinAState["));Serial.print(i,DEC); Serial.print(F("]=")); Serial.println(pinAState[i]);}
  }
*/






//////////////////////////////////AVR ==> NodeMCU speach this need to be used later to apply the source of truth concept :D
/*
void settingToNodeMCU(){

  DynamicJsonDocument doc(100);
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

serializeJson(doc, Serial);
Serial.println();
doc.clear();
delay(200);
}

for(unsigned char i=0; i<digitalPointer ;i++){
String Dn="D";  
Dn+=String(digitalPin[i]);
doc["H"] = "s";
JsonObject D_Config = doc.createNestedObject(Dn);
JsonArray D_modes = D_Config.createNestedArray("modes");
//D_modes.add("a");
D_modes.add("i");
D_modes.add("o");
//D_Config["mode"] =NULL;  
//D_Config["int"] = NULL;  //// there is no pwd in
if(pwmPin[i])D_modes.add("p");
//JsonObject D_Config = doc.createNestedObject(Dn);
doc[Dn]["mode"]=  String(digitalMode[i]);
doc[Dn]["int"] =  digitalInterval[i];  //// there is no pwd in

serializeJson(doc, Serial);
Serial.println();
doc.clear();
delay(200);
}



for(unsigned char i=0; i<AnIndex ;i++){
String An="A";
An+=String(i);
doc["H"] = "s";
JsonObject A_Config = doc.createNestedObject(An);
JsonArray A_modes = A_Config.createNestedArray("modes");
A_modes.add("a");
A_modes.add("i");
A_modes.add("o");
A_Config["mode"] =NULL;
A_Config["int"] = NULL;  //// there is no pwd in


serializeJson(doc, Serial);
Serial.println();
doc.clear();
delay(200);

}

for(char i=0;i<analogPointer;i++){
String An="A"; 
An+=String(analogPin[i]-14);
//JsonObject A_Config = doc.createNestedObject(An);
doc[An]["mode"]=  String(analogMode[i]);
doc[An]["int"] =  analogInterval[i];  //// there is no pwd in

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
///////////////////
*/

//archieved due to the cloud integration
/*
void sendDUpdate(unsigned char pin){
  if(digitalMode[pin]=='i'){pinState[digitalPin[pin]] =digitalRead(digitalPin[pin]); Serial.println(F("GetIn"));}
  Serial.print(F("update:D"));
  Serial.print(digitalPin[pin]);
  Serial.print(F("="));
  Serial.println(pinState[digitalPin[pin]]);
//  digitalUFrame(pin, digitalMode[pin]);
//  digitalUFrame('Y', 'o'); //for test
  sendUFrame(digitalMode[pin]);
  }

void sendAUpdate(unsigned char pin){
  unsigned char pinNumber=analogPin[pin]-14;
  if(analogMode[pin]=='i')     {pinAState[pinNumber] = digitalRead(analogPin[pin]); Serial.println(F("GetIn"));}
  else if(analogMode[pin]=='a'){pinAState[pinNumber] = analogRead(analogPin[pin]); Serial.println(F("GetIn"));}
  Serial.print(F("update:A"));
  Serial.print(pinNumber);
  Serial.print(F("="));
  Serial.println(pinAState[pinNumber]);
//  analogUFrame(pin,analogMode[pin]);
//  analogUFrame('Y','a');
  sendUFrame(analogMode[pin]);

  } 
/////////////////////////////////////////


///////////////////////////////////// archieved due to the cloud integration
// archieved updates frame
void analogUFrame(unsigned char pin,unsigned char mode){
 
  DynamicJsonDocument doc(200); 
  doc["H"] = "u";
  
  if(pin == 'Y'){ // if allpin argument == yes send all digital values with the passed mode in , out, or analog 
   for(unsigned char i =0;i<analogPointer; i++){
    unsigned char pinNumber=analogPin[i]-14;
//    Serial.print("pinNumber: ");
//    Serial.print(pinNumber);
//    Serial.print(" analogMode: ");
//    Serial.println(analogMode[i]);
    String An="A";
    An+=String(pinNumber);
//    if(mode == analogMode[i]){doc[An] = pinAState[pinNumber];} // to fillter by mode
    doc[An] = pinAState[pinNumber];
    }
  }
  else{
    unsigned char pinNumber=analogPin[pin]-14;
    String An="A";
    An+=String(pinNumber);
    doc[An] = pinAState[pinNumber];
    }
    serializeJson(doc, Serial);
    Serial.println();
}

 
void digitalUFrame(unsigned char pin, unsigned char mode){
  DynamicJsonDocument doc(200); 
  doc["H"] = "u";
  
  if(pin == 'Y'){ // if allpin argument == yes send all digital values with the passed mode in , out, or PWM
  for(unsigned char i =0;i<digitalPointer;i++){
    String Dn="D";
    Dn+=String(digitalPin[i]);
    //if(mode == digitalMode[i]){doc[Dn] = pinState[digitalPin[i]];} // two fitler the by mode
    doc[Dn] = pinState[digitalPin[i]];
    }
  }
  else{
    String Dn="D";
    Dn+=String(digitalPin[pin]);
    doc[Dn] = pinState[digitalPin[pin]];
    }
    serializeJson(doc, Serial);
    Serial.println();
}
*/


  
  
