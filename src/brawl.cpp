#include "brawl.h"

#include "main.h"
#include "util.h"
#include "OTA.h"
#include "webserver2.h"
#include <Arduino.h>
#include <HTTPClient.h>

#define DEBUGGING 1


void changeBrawlInfo(){
  webPage=htmlPage;
  if(notice) free(notice);
  unsigned int noticeLength = (int) server.arg("myText").length();
  notice = (char *) malloc(sizeof(char) * (noticeLength+1));
  server.arg("myText").toCharArray(notice, noticeLength);
  notice[noticeLength] = '\0';
  
  myPrintln("Text Received, contents:");
  myPrintln(notice);

  free(notice);
  server.send(200,"text/html",webPage);  
}

bool handleBrawlData(){
  switch(brawlState){
    case 1: brawlSendRequest();
            brawlState++;
            return true;
    case 2: brawlListenForResponse();
            brawlState++;
            return true;
    case 3: brawlParseJson();
            brawlState = 0;
            return true;
    default:
            break;
  }
  
  // get new data every 3 minutes
  if((millis()/1000.0-lastBrawlQuery) > 180.0){
    
    #if defined(DEBUGGING)
    Telnet.println((millis()/1000.0)-lastBrawlQuery);
    Telnet.println(millis());
    #endif

    brawlConnectServer();
    brawlState++;
    lastBrawlQuery = (millis()/1000.0);
    return true;
  }
  return false;
}

void brawlConnectServer(){
  myPrint("\n[Connecting to ");
  myPrintln(brawlServername);

  // brawlClient.setInsecure()

  // brawlClient.connect(brawlServername, 443);  //starts client connection, checks for connection
  
  // while(!brawlClient.connected()) delay(10);

  #if defined(DEBUGGING)
  Telnet.println("connected]");
  #endif
}

void brawlSendRequest(){
  #if defined(DEBUGGING)
  Telnet.println("[Sending a request]");
  #endif
  
  // brawlClient.print(String("GET /brawl/players/") + playerID + " HTTP/1.1\r\n" +
      // "Host: " + brawlServername + "\r\n" +
  //     "Connection: close\r\n" +
  //     "\r\n"
  //     );

}


String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void brawlListenForResponse(){
  #if defined(DEBUGGING)
  Telnet.println("[Response:]");
  #endif
  
  // while (brawlClient.connected()) {
  //   readResult = brawlClient.readStringUntil('\n');
    
  //   Telnet.println(readResult);

  //   if (readResult == "\r") {
  //     #if defined(DEBUGGING)
  //     Telnet.println("headers received");
  //     #endif
  //     break;
  //   }

  // }
  const char* servername = "https://esping-brawl-api.herokuapp.com/brawl/players/PC2J0V08V";
  String readResult = httpGETRequest(servername);

  Telnet.println(readResult);

  // readResult = brawlClient.readStringUntil('\n');
  if(brawlResult != NULL){
    free(brawlResult);
  }
  brawlResult = (char *) malloc((readResult.length()+1) * sizeof(char));
  readResult.toCharArray(brawlResult, readResult.length()+1);
  //    brawlResult[readResult.length()+1] = '\0';
  #if defined(DEBUGGING)
  Telnet.println(brawlResult);
  #endif

  // brawlClient.stop(); //stop client
}



void brawlParseJson(){

    char* jsonPart = partOfString(brawlResult, 50, 300);
    brawlInfoOnServer = getComponentFromJson(brawlResult, identifier, brawlername);
    brawlInfoOnDisplay = atoi(brawlInfoOnServer);
    Telnet.println(brawlInfoOnServer);
    Telnet.println("parsed!");
}
