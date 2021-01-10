#include <Arduino.h>
#include <mDNS.h>
#include <HTTPClient.h>

extern char const * brawlServername;
extern String playerID;
extern int32_t lastBrawlQuery;
extern int brawlState;
extern char* brawlResult;
extern char* brawlInfoOnServer;
extern int brawlInfoOnDisplay;
extern char const * brawlername;
extern char const * identifier;
extern WiFiClient brawlClient;

extern const char* webPage;
extern char* notice;
extern const char htmlPage[]PROGMEM;


void changeBrawlInfo();
bool handleBrawlData();
void brawlConnectServer();
void brawlSendRequest();
void brawlListenForResponse();
void brawlParseJson();