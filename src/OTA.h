#include <cstdint>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

extern bool ota_flag;
extern uint16_t time_elapsed;
extern WiFiServer telnetServer;
extern WiFiClient Telnet;

void setupOTA();
void handleOTA();
void setupTelnet();
void handleTelnet();
