#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

extern const char* ssid;
extern const char* password;
extern ESP8266WebServer server;
extern bool ota_flag;
extern uint16_t time_elapsed;
extern float temperature;
extern char* brawlInfoOnServer;
extern const int utcOffsetInSeconds;
extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

extern long colorBG;
extern long colorFG;
extern int activeMode;
extern int activeBackground;
enum modes {TIME, WEATHER, BRAWLSTARS, SLIDESHOW, SNAKE, PONG};
enum backgrounds {RAINBOW, STATIC};

void wifiSetup();
void serverSetup();