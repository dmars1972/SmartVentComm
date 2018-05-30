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

  client.println(hostName);
  client.write(t);

  client.flush();

  do {
    delay(50);
    id = client.read();
  } while(id == 255);

Serial.print("Assigned ID: ");
Serial.println(id, DEC);
  client.stop();

  return true;
}

bool SmartVentComm::getRegistration(RegisterStruct *r)
{
  unsigned char nextID = 1;
  byte x;
  WiFiClient client = regServer.available();

  if(!client) 
    return false;

  client.readBytesUntil('\n', r->host, sizeof(r->host));
  r->hasSensor = client.read();

  Serial.print("Assigning ID ");
  Serial.println(nextID);

  client.write(nextID);

  client.flush();

  client.stop();

  return true;
}

bool SmartVentComm::receiveTemperature(unsigned char *id, unsigned char *t)
{
  WiFiClient client = comServer.available();
  unsigned char i;
  unsigned char j;

  if(!client) 
    return false;

Serial.println("Receiving");

  do {
    delay(50);
    i = client.read();
  } while(i == 255);

  do {
    delay(50);
    j = client.read();
  } while(j == 255);

Serial.print("Got ");
Serial.print(i, DEC);
Serial.print(" ");
Serial.println(j, DEC);

  id = &i;
  t = &j;
  client.stop();

  return true;
}

bool SmartVentComm::sendTemperature(unsigned char t)
{
  uint32_t elapsed;
  WiFiClient client;

  client.connect("thermostat", comPort);

  elapsed = millis();

  while(!client.connected()) {
    if(millis() > elapsed + 30000) {
      Serial.println("Connect failed after 30 seconds, aborting.");
      return false;
    }
    delay(100);
  }
 
Serial.print("Sending ");
Serial.print(id, DEC);
Serial.print(" ");
Serial.println(t);

  client.write(id);

  client.write(t);

  client.flush();

  client.stop();

  return true;
}
