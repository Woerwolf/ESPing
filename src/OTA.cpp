#include "OTA.h"

// #include <Arduino.h>
#include <ArduinoOTA.h>

// #define DEBUGGING 1

void setupOTA(){
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
    #if defined(DEBUGGING)
    ArduinoOTA.onStart([]() {
        char* type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
        } else { // U_SPIFFS
        type = "filesystem";
        }

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Telnet.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Telnet.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Telnet.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Telnet.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
        Telnet.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
        Telnet.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
        Telnet.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
        Telnet.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
        Telnet.println("End Failed");
        }
    });
    #endif
    ArduinoOTA.begin();
}

void handleOTA(){
    if(ota_flag)
    {
        uint16_t time_start = millis();
        while(time_elapsed < 10000)
        {
        ArduinoOTA.handle();
        time_elapsed = millis()-time_start;
        delay(10);
        }
        ota_flag = false;
    }
}

void setupTelnet(){
    telnetServer.begin();
    telnetServer.setNoDelay(true);
}

void handleTelnet(){
  if(telnetServer.hasClient()){
    //client is connected
    if(!Telnet || !Telnet.connected()){
      //client disconnected
      if(Telnet) Telnet.stop();
      Telnet = telnetServer.available();
    }else{
      telnetServer.available().stop();
    }
  }
}