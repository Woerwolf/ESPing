#include "brawl.h"

#include "util.h"
#include "OTA.h"
#include "webserver.h"
#include <Arduino.h>

#define DEBUGGING 1


void changeBrawlInfo(){
  webPage=htmlPage;
  if(notice) free(notice);
  unsigned int noticeLength = (int) server.arg("myText").length();
  notice = (char *) malloc(sizeof(char) * (noticeLength+1));
  server.arg("myText").toCharArray(notice, noticeLength);
  notice[noticeLength] = '\0';
  
  #if defined(DEBUGGING)
  Telnet.println("Text Received, contents:");
  Telnet.println(notice);
  #endif

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
  #if defined(DEBUGGING)
  Telnet.println("\n[Connecting to " + brawlServername);
  #endif

  //  const char fingerprint[] = "5C 67 CF 09 6C 0F AE 2B CE D4 29 73 F3 FC 01 23 A7 58 84 43";
  //  brawlClient.setFingerprint(fingerprint);
  brawlClient.setInsecure();
    
  brawlClient.connect(brawlServername, 443);  //starts client connection, checks for connection
  
  #if defined(DEBUGGING)
  Telnet.println("connected]");
  #endif
}

void brawlSendRequest(){
  #if defined(DEBUGGING)
  Telnet.println("[Sending a request]");
  #endif
  
  brawlClient.print(String("GET /brawl/players/") + playerID + " HTTP/1.1\r\n" +
      "Host: " + brawlServername + "\r\n" +
      "Connection: close\r\n" +
      "\r\n"
      );

}

void brawlListenForResponse(){
  #if defined(DEBUGGING)
  Telnet.println("[Response:]");
  #endif

  String readResult;
  while (brawlClient.connected()) {
    readResult = brawlClient.readStringUntil('\n');
    if (readResult == "\r") {
      #if defined(DEBUGGING)
      Telnet.println("headers received");
      #endif
      break;
    }
  }
  readResult = brawlClient.readStringUntil('\n');
  if(brawlResult){
    free(brawlResult);
  }
  brawlResult = (char *) malloc((readResult.length()+1) * sizeof(char));
  readResult.toCharArray(brawlResult, readResult.length());
  //    brawlResult[readResult.length()+1] = '\0';

  #if defined(DEBUGGING)
  Telnet.println(brawlResult);
  #endif

  brawlClient.stop(); //stop client
}

void brawlParseJson(){

    char* jsonPart = partOfString(brawlResult, 50, 200);
    free(brawlInfoOnServer);
    brawlInfoOnServer = getComponentFromJson(brawlResult, identifier, brawlername);
    free(jsonPart);

}
