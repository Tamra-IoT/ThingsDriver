#ifndef __MQTT_TAMRA_H__
#define __MQTT_TAMRA_H__
#include"tamra_nodemcu.h"
void catchFrames();
void uploadToCloud(String topic);
//void createFullTopicDirectories();
void setWellFlag();
void checkBrokerConnection();
void reconnect();
void callback1();
void setup_MQTT();

#endif
