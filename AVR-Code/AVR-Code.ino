/*
1-Deal with big JSON --> deserialize the comming JSON--->interpret setting and command frames-> execute setting and command frames
2-Remove factor parmmters
3- adding Second timer
Hints:
i- Naming of pinState and pinAstate is complicated to made a relation between the pin number and it's oldstate 
to check if there is an update or it repeated command. I'm not sure if there is more simple way or no =D 
bug reported at 21 of march
Add line to send empty settings in the setup
discover criticil bug==> don't use pin 9 and 10 as PWM due to the Timer interrupt confilct  
https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
*/
/////////////EEPROM configurations
/////////////EEPROM configurations
#include "AVR-Code.h"

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
extern bool tick;
////////////////////////
extern unsigned short ROMAddress;

bool startReceivingSetting=true;
////////////////////////////////////////////////////////////////////////////////////////////////////
//void initialize_timer()
//{
// cli();          // disable global interrupts
//  
//  // initialize Timer1 for interrupt @ 1000 msec
//  TCCR1A = 0;     // set entire TCCR1A register to 0
//  TCCR1B = 0;     // same for TCCR1B
//  // set compare match register to desired timer count:
//  OCR1A = 15624;
//  // turn on CTC mode:
//  TCCR1B |= (1 << WGM12);
//  // Set CS10 and CS12 bits for 1024 prescaler:
//  TCCR1B |= (1 << CS10);
//  TCCR1B |= (1 << CS12);
//  // enable timer compare interrupt:
//  TIMSK1 |= (1 << OCIE1A);
//  
//  sei();          // enable global interrupts
//}

////////////////////////////////////////////////////////////////////////////////////////////////////



// timer interrupt routine
//ISR(TIMER1_COMPA_vect)
//{ 
//  //sensortimer++;  
//  tick=true;
//}
  
  
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
  Serial.begin(115200);
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
  else{settingToNodeMCU();} // to send applied=true element to sync with the NodeMCU
//else{updateSettings();        Serial.println("update called");} for debuging purpose
//////////////////////////////////
 //delay(30000); //wait to estiblish the wifi connection and mqtt server 
 do{}while(!Serial.available() || Serial.read() != 'R');
 Serial.flush();
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
    updatePortsStates();
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
