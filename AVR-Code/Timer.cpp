#include "Timer.h"
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
//timer variables
bool tick = false;
////////////////////////////////////////////////////////////////////////////////////////////////////
void initialize_timer()
{
  cli(); // disable global interrupts

  // initialize Timer1 for interrupt @ 1000 msec
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);

  sei(); // enable global interrupts
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// timer interrupt routine
ISR(TIMER1_COMPA_vect)
{
  //sensortimer++;
  tick = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////
void TurnTimersON()
{
  TIMSK1 |= (1 << OCIE1A); //enable timer1
}

void TurnTimersOFF()
{
  TIMSK1 &= ~(1 << OCIE1A); // turn off the timer interrupt
}

////////////////////////////////////////////
void checkAlarms()
{
  for (unsigned char i = 0; i < digitalPointer; i++)
  {
    if (digitalTimer[i] >= digitalInterval[i])
    {
      //sendDUpdate(i);  //archieved due to the cloud integration
      updatePortsStates(); // this function should rename because it's get and update all the states
      if (digitalMode[i] == 'i')
      {
        sendUFrame(digitalMode[i]);
      }
      digitalTimer[i] = 0;
    }
  }
  for (unsigned char i = 0; i < analogPointer; i++)
  {
    if (analogTimer[i] >= analogInterval[i])
    {
      //      sendAUpdate(i); //archieved due to the cloud integration
      updatePortsStates(); // this function should rename because it's get and update all the states
      if (analogMode[i] == 'i' || analogMode[i] == 'a')
      {
        sendUFrame(analogMode[i]);
      }
      analogTimer[i] = 0;
    }
  }
}
