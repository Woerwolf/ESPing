/* Blink Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#define DEBUGGING_ONLINE 1
#define DEBUGGING_OFFLINE 1

#pragma region include
  //library includes
  #include <WiFi.h>
  #include <mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <WebServer.h>
  #include <ArduinoJson.h>
  #include <HTTPClient.h>
  #include <NTPClient.h>
  #include <Ticker.h>

  //header file
  #include "main.h"
  //includes for freertos
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  //espidf includes
  #include "driver/gpio.h"
  #include "esp_system.h"
  #include "sdkconfig.h"
  //local includes
  #include "OTA.h"
  #include "weather.h"
  #include "brawl.h"
  #include "util.h"
  #include "webserver2.h"
  #include "display.h"
  #include "background.h"
  #include "credentials.h"
#pragma endregion include

#pragma region globalVariables

// defined in webserver2.h
// enum modes {OFFLINE, CLEAR, TIME, WEATHER, BRAWLSTARS, SLIDESHOW, SNAKE, PONG};
// enum backgrounds {RAINBOW, STATIC};
long colorBG;
long colorFG;
int activeMode = OFFLINE;
int activeBackground = STATIC;

Ticker blinkerSeconds;

const char* ssid = SSID;
const char* password = PW;

CRGB leds[ledQuantity];
CRGB* display[ledRows][ledColumns];
int lastTimeShown = -1;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
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

char const * brawlServername = BRAWLAPISERVER;
String playerID = "PC2J0V08V";
// String playerID = "RJGPYVP9";
// String playerID = "9R8Y820P";
int32_t lastBrawlQuery = -6000;
int brawlState = 0; // 0: no connection to api      1: connected      2: requested      3: answer received      4=0: answer parsed/no connection to api

char* brawlResult;
char* brawlInfoOnServer;
int brawlInfoOnDisplay = 0;

char const * brawlername = "{";
char const * identifier = "trophies";

char const * weatherServername = "api.openweathermap.org";
const String cityID = CITYID;
const String  apiKey = WEATHERAPIKEY;
int32_t lastWeatherQuery = -600;
String weatherResult = "";
float temperature = 0;

WebServer server(80);
WiFiClient brawlClient;
WiFiClient weatherClient;

WiFiServer telnetServer(23);
WiFiClient Telnet;

bool ota_flag = false;
uint16_t time_elapsed = 0;

#pragma endregion globalVariables

#pragma region taskVariables
//naming scheme: taskname = "task" + taskNr + funtion;
TaskHandle_t task999Hndlr = NULL;
static void task999BlinkLed(void * pvParameters);

TaskHandle_t task1Hndlr = NULL;
static void task1DispOut(void * pvParameters);

TaskHandle_t task2Hndlr = NULL;
static void task2DataFetch(void * pvParameters);

#pragma endregion taskVariables

void setup(void) {
  if(wifiSetup()){
    serverSetup();
    setupOTA();
    setupTelnet();

    Serial.begin(115200);

    activeMode = CLEAR;
    activeBackground = RAINBOW;
    colorFG = 0xffffff;
    colorBG = 0x999999;


    timeClient.begin();
    timeClient.forceUpdate();  
    lastTimeUpdate = timeClient.getMinutes();
    // char* formattedDate = &timeClient.getFormattedDate()[0];
    // char* date = strtok(formattedDate, "T");
    // int day = atoi(strtok(date, "-"));
    // int month = atoi(strtok(NULL, "-"));
    // int year = atoi(strtok(NULL, "-"));
    // if((month>3 && month<10) || (month==3 && day+7-(1+getWeekday(day, month, year)%7)>31 || month==10) && (day+7-(getWeekday(day, month, year)%7)<31)) {
      // timeClient.setTimeOffset(7200);
    // }
    // else {
      timeClient.setTimeOffset(3600);
    // }
  }
  

  setupLED();  

  FastLED.showColor(0x00ff00);

  blinkerSeconds.attach(0.01, handleTimerInterrupt);
}


void task999BlinkLed(void * pvParameters){
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        while(1) {
		printf("GPIO:%i goes high\n",BLINK_GPIO);
    GPIO_OUTPUT_SET( BLINK_GPIO, 1 );
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		printf("GPIO:%i goes low\n", BLINK_GPIO);
		GPIO_OUTPUT_SET( BLINK_GPIO, 0 );
		vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
    }
}

void task1DispOut(void * pvParameters){
  switch(activeBackground){
    case RAINBOW: 
                  {
                  int rainbowBrightness = ((colorBG%256)+((colorBG/256)%256)+(colorBG%256%256))/3;
                  rainbowbg(3.1, rainbowBrightness);
                  break;
                  }
    case STATIC:
                  staticbg(colorBG);
                  break;
    default:      
                  staticbg(0x000000);
                  break;
  }
  
  switch(activeMode){
    case OFFLINE:
                animateWifiError(1,8, 0xff0000);
                break;
    case TIME: 
                writeTime(timeClient.getHours(), timeClient.getMinutes(), colorFG);
                FastLED.show();
                break;
    case WEATHER:
                writeTemp(temperature, colorFG);
                FastLED.show();
                break;
    case BRAWLSTARS:
                if(brawlResult == NULL) break;
                writeNumber(brawlInfoOnDisplay, colorFG);
                FastLED.show();
                break;
    default:
                FastLED.show();
                break;
  }
}

void task2DataFetch(void * pvParameter){
  switch(activeMode){
    case TIME: 
                if((timeClient.getMinutes()+60 - lastTimeUpdate)%60 > 10){
                  timeClient.forceUpdate();  
                  lastTimeUpdate = timeClient.getMinutes();
                }
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

  if(activeMode != OFFLINE){
    handleOTA();
    handleTelnet();

    server.handleClient();
  }
  delay(10);
}

void app_main(void)
{   
  setup();

  xTaskCreatePinnedToCore(task999BlinkLed, "BlinkLed", 256, NULL, 2, task999Hndlr, 0);
  xTaskCreatePinnedToCore(task1DispOut, "DisplayOutput", 1024, NULL, 2, task1Hndlr, 1);
  xTaskCreatePinnedToCore(task2DataFetch, "DataFetch", 2048, NULL, 3, task2Hndlr, 0)
}
