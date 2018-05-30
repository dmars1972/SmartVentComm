#ifndef SMART_VENT_COMM
#define SMART_VENT_COMM

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>

struct RegisterStruct {
  char host[15];
  unsigned char hasSensor;
};

class SmartVentComm
{
  private:
    unsigned char id;
    unsigned char macAddress[6];
    char hostName[15];
    char ssid[64];
    char password[64];

    bool connected;

    uint16_t regPort;
    uint16_t comPort;
    WiFiServer regServer;
    WiFiServer comServer;

    RegisterStruct r;
  public:
    SmartVentComm(uint16_t, uint16_t);

    // getters
    bool isConnected() { return connected; };

    // methods
    bool registerVent(unsigned char);
    bool sendTemperature(unsigned char);
    bool receiveTemperature(unsigned char *, unsigned char *);
    bool getRegistration(RegisterStruct *);
    
};

#endif
