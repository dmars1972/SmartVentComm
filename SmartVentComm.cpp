#include "SmartVentComm.h"

SmartVentComm::SmartVentComm(uint16_t rp, uint16_t cp)
:regServer(rp), comServer(cp)
{
  memset(hostName, '\0', sizeof(hostName));

  WiFi.macAddress(macAddress);

  sprintf(hostName, "sv%02x%02x%02x%02x%02x%02x", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);

  regPort = rp;
  comPort = cp;

  regServer.begin();
  comServer.begin();

  return;
}

bool SmartVentComm::registerVent(unsigned char t)
{
  uint32_t elapsed;
  WiFiClient client;

  client.connect("thermostat", regPort);

  elapsed = millis();

  while(!client.connected()) {
    if(millis() > elapsed + 30000) {
      Serial.println("Connect failed after 30 seconds, aborting.");
      return false;
    }
    delay(100);
  }

  client.print(hostName);
  client.write(t);

  client.flush();

  client.stop();

  return true;
}

bool SmartVentComm::getRegistration(RegisterStruct *r)
{
  WiFiClient client = regServer.available();

  if(!client) 
    return false;

  memset(r->host, '\0', sizeof(r->host));

  client.readBytes(r->host, sizeof(r->host)-1);
  r->hasSensor = client.read();

  client.stop();

  return true;
}

bool SmartVentComm::receiveTemperature(TempStruct *t)
{
  WiFiClient client = comServer.available();
  unsigned char i;
  unsigned char j;

  if(!client) 
    return false;

  client.readBytes(t->host, sizeof(t->host)-1);
  t->host[sizeof(t->host)] = '\0';

  do {
    delay(50);

    t->temp = client.read();
  } while(t->temp == 255);

  client.stop();

  return true;
}

bool SmartVentComm::sendTemperature(unsigned char t)
{
  uint32_t elapsed;
  WiFiClient client;
  TempStruct ts;

  client.connect("thermostat", comPort);

  elapsed = millis();

  while(!client.connected()) {
    if(millis() > elapsed + 30000) {
      Serial.println("Connect failed after 30 seconds, aborting.");
      return false;
    }
    delay(100);
  }
 
  strcpy(ts.host , hostName);
  ts.temp = t;

Serial.print("Sending ");
Serial.print(ts.host);
Serial.print(" ");
Serial.println(ts.temp, DEC);

  client.print(ts.host);

  client.write(ts.temp);

  client.flush();

  client.stop();

  return true;
}

bool SmartVentComm::getCommand(unsigned char *c)
{
  WiFiClient client = comServer.available();

  if(!client)
    return false;

  c = (unsigned char *)client.read();
 
  client.stop();

  return true;
}

bool SmartVentComm::sendCommand(const char *h, unsigned char c)
{
  uint32_t elapsed;
  WiFiClient client;

  client.connect(h, comPort);

  elapsed = millis();

  while(!client.connected()) {
    if(millis() > elapsed + 10000) {
      Serial.println("Connect failed after 10 seconds, aborting.");
      return false;
    }
    delay(100);
  }
 
  client.write(c);
  client.flush();
  client.stop();

  return true;
}
