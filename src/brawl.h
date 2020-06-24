#include <Arduino.h>
#include <ESP8266mDNS.h>

extern const String brawlServername;
extern String playerID;
extern int32_t lastBrawlQuery;
extern int brawlState;
extern char* brawlResult;
extern char* brawlInfoOnServer;
extern char const * brawlername;
extern char const * identifier;
extern WiFiClientSecure brawlClient;

extern const char* webPage;
extern char* notice;
extern const char htmlPage[]PROGMEM;


void changeBrawlInfo();
bool handleBrawlData();
void brawlConnectServer();
void brawlSendRequest();
void brawlListenForResponse();
void brawlParseJson();