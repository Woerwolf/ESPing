#include <cstdint>
#include <WebServer.h>
#include <WiFi.h>

extern bool ota_flag;
extern uint16_t time_elapsed;
extern WiFiServer telnetServer;
extern WiFiClient Telnet;

void setupOTA();
void handleOTA();
void setupTelnet();
void handleTelnet();
