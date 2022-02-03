#ifndef __PORT_SETTINGS_H__
#define __PORT_SETTINGS_H__

#include "AVR-Code.h"

#define DnIndex 14
#define AnIndex 6


void digitalSetup();
void analogSetup();
void sendUFrame(char mode);
void updatePortsStates();
void getSavedSettings();
void updateSettings();

#endif
