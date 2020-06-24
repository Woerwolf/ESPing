#include "webserver.h"

#include "OTA.h"
#include "brawl.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define DEBUGGING 1


void wifiSetup(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Telnet.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    #if defined(DEBUGGING)
    Telnet.println("Ready");
    Telnet.print("IP address: ");
    Telnet.println(WiFi.localIP());
    #endif
}

void handleModeRequest(){
    if (server.hasHeader("Mode")){
        if(server.header("Mode").equals("time")){
            activeMode = TIME;
        }
        if(server.header("Mode").equals("brawl")){
            activeMode = BRAWLSTARS;
        }
        if(server.header("Mode").equals("weather")){
            activeMode = WEATHER;
        }
    }

    if (server.hasHeader("Background")){
        if(server.header("Background").equals("rainbow")){
            activeBackground = RAINBOW;
        }
        if(server.header("Background").equals("static")){
            activeBackground = STATIC;
        }
    }

    if (server.hasHeader("ColorBG")){
        char colorCharArray[6];
        server.header("ColorBG").toCharArray(colorCharArray, 6);
        colorBG = strtol(colorCharArray, NULL, 16);
    }

    if (server.hasHeader("ColorFG")){
        char colorCharArray[6];
        server.header("ColorFG").toCharArray(colorCharArray, 6);
        colorBG = strtol(colorCharArray, NULL, 16);
    }

    server.send(200,"text/plain", "Mode request received!");
}

void serverSetup(){
    server.on("/restart",[](){
        server.send(200,"text/plain", "Restarting...");
        delay(1000);
        ESP.restart();
    });

    server.on("/setflag",[](){
        server.send(200,"text/plain", "Setting flag...");
        ota_flag = true;
        time_elapsed = 0;
    });

    server.on("/weather", [](){
        server.send(200, "text/plain", String(temperature));
    });

    server.on("/mytrophies", [](){
        if(brawlInfoOnServer){
        server.send(200, "text/plain", String(brawlInfoOnServer));
        }else{
        server.send(200, "text/plain", "error");
        }
    });

    server.on("/changeBrawlInfo", changeBrawlInfo);

    server.on("/time", [](){
        server.send(200, "text/plain", String(timeClient.getFormattedTime()));
    });

    server.on("/mode", handleModeRequest);

    const char * headerkeys[] = {"Mode", "Background", "ColorBG", "ColorFG"} ;
    size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
    //ask server to track these headers
    server.collectHeaders(headerkeys, headerkeyssize );

    server.begin();
}