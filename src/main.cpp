/*------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to program the ESP8266 Over The Air 
  (OTA).
  https://youtu.be/gFK2EDNpIeM
  https://youtu.be/R3aB85PuOQhY

  Brawlstars API:
  https://developer.brawlstars.com/#/

  Open Weather Map API:
  https://openweathermap.org/api

------------------------------------------------------------------------------*/

// #define DEBUGGING 1

#pragma region include
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <ESP8266WebServer.h>
  #include <ArduinoJson.h>
  #include <ESP8266HTTPClient.h>
  #include <NTPClient.h>
  #include <Ticker.h>

  #include "OTA.h"
  #include "weather.h"
  #include "brawl.h"
  #include "util.h"
  #include "webserver.h"
  #include "display.h"
  #include "background.h"
  #include "credentials.h"
#pragma endregion include

#pragma region globalVariables

// defined in webserver.h
// enum modes {TIME, WEATHER, BRAWLSTARS, SLIDESHOW, SNAKE, PONG};
// enum backgrounds {RAINBOW, STATIC};
long colorBG = 0x00bbbb;
long colorFG = 0xffffff;
int activeMode = TIME;
int activeBackground = STATIC;

Ticker blinkerSeconds;

const char* ssid = SSID;
const char* password = PW;

NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart1800KbpsMethod> strip(ledQuantity, dataPin);

const int utcOffsetInSeconds = 7200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
// last time update at minute:
int lastTimeUpdate = 0;

const char* webPage;
char* notice;
const char htmlPage[]PROGMEM=R"=====(
  <!DOCTYPE html>
  <html>
  <body>
  <h3>Change info displayed at /mytrophies </h3>
  <FORM METHOD="POST"action="/changeBrawlInfo">
  <input type="text" name="myText" value="Write your Notice...">
  <input type="submit" value="Post Notice">
  </form>
  </body>
  </html>
  )=====";

const String brawlServername = "esping-brawl-api.herokuapp.com";
String playerID = "PC2J0V08V";
// String playerID = "RJGPYVP9";
// String playerID = "9R8Y820P";
int32_t lastBrawlQuery = -60;
int brawlState = 0; // 0: no connection to api      1: connected      2: requested      3: answer received      4=0: answer parsed/no connection to api

char* brawlResult;
char* brawlInfoOnServer;

char const * brawlername = "{";
char const * identifier = "trophies";

const String weatherServername="api.openweathermap.org";
const String cityID = "6556348";
const String  apiKey = "77251286a8b3a2888376d53c4ca3e826";
int32_t lastWeatherQuery = -600;
String weatherResult = "";
float temperature;

ESP8266WebServer server;
WiFiClientSecure brawlClient;
WiFiClient weatherClient;

WiFiServer telnetServer(23);
WiFiClient Telnet;

bool ota_flag = true;
uint16_t time_elapsed = 0;

#pragma endregion globalVariables

void handleTimerInterrupt(){
  switch(activeBackground){
    case RAINBOW: 
                  rainbowbg(0.15, 170);
                  break;
    case STATIC:
                  staticbg(colorBG);
                  break;
    default:      
                  staticbg(0x000000);
                  break;
  }
  
  switch(activeMode){
    case TIME: 
                writeTime(timeClient.getHours(), timeClient.getMinutes(), 0xffffff);
                break;
    case WEATHER:
                writeTemp(temperature, 0xffffff);
                break;
    case BRAWLSTARS:
                if(brawlResult == NULL) break;
                writeNumber(atoi(getComponentFromJson(brawlResult, "trophies", "{")), 0xffffff);
                break;
    default:
                break;
  }

}

void setup() {
  setupOTA();
  setupTelnet();

  setupLED();  

  timeClient.begin();
  timeClient.forceUpdate();  
  lastTimeUpdate = timeClient.getMinutes();
  
  wifiSetup();
  serverSetup();

  #if defined(DEBUGGING)
    Telnet.println("Booting");
  #endif


  // blinkerSeconds.attach(0.1, handleTimerInterrupt);
}

void loop() {

  
  #if defined(DEBUGGING)
  Telnet.println("loop");
  #endif

  switch(activeMode){
    case TIME: 
                timeClient.update();  
                break;
    case WEATHER:
                handleWeatherData();
                break;
    case BRAWLSTARS:
                handleBrawlData();
                break;
    default:
                break;
  }

  switch(activeBackground){
    case RAINBOW: 
                  rainbowbg(0.15, 170);
                  break;
    case STATIC:
                  staticbg(colorBG);
                  break;
    default:      
                  staticbg(0x000000);
                  break;
  }
  
  switch(activeMode){
    case TIME: 
                writeTime(timeClient.getHours(), timeClient.getMinutes(), 0xffffff);
                break;
    case WEATHER:
                writeTemp(temperature, 0xffffff);
                break;
    case BRAWLSTARS:
                if(brawlResult == NULL) break;
                writeNumber(atoi(getComponentFromJson(brawlResult, "trophies", "{")), 0xffffff);
                break;
    default:
                break;
  }

  handleOTA();
  handleTelnet();

  server.handleClient();

  delay(30);
}