#ifndef __TAMRA_NODEMCU_H__
#define __TAMRA_NODEMCU_H__
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
////////////////////////////////////////////
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "Arduino.h"
#include "wifi_tamra.h"
#include "mqtt_tamra.h"
#include "serial_mcutamra.h"
#include "jsonHandler_mcuTamra.h"
#include <EEPROM.h>
#define NodeIDlength 24
//#define RestWifi_Button 5 // 5 is GPIO5(D1) see this link => https://microcontrollerslab.com/led-blinking-using-esp8266-nodemcu/
#define RestWifi_Button 4 // 4 is GPIO4 (D2) see this link => https://microcontrollerslab.com/led-blinking-using-esp8266-nodemcu/
#define DnIndex 14
#define AnIndex 6
#define wifi_name "Nawa Wifi"
#define node_port 5678
#define http_port 80
/////////////////////////////////////////////////
#define stateInterval 60
#define Qos 1
#endif
