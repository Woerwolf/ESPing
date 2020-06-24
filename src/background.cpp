#include "background.h"

#include "display.h"
#include <FastLED.h>

#define DEBUGGING 1

void rainbowbg(float speed, int brightness){
    unsigned int baseHue = int(millis()*speed)%256;

    for(int col = 0; col<ledColumns; col++){
        unsigned int hue = (baseHue+col*10)%256;
        for(int row = 0; row<ledRows; row++){
            if(display[row][col] != NULL){
                *display[row][col] = CHSV(hue, 255, brightness);
            }
        }
    }
}

void staticbg(long color){
    for(int col = 0; col<ledColumns; col++){
        for(int row = 0; row<ledRows; row++){
            if(display[row][col] != NULL){
                *display[row][col] = color;
            }
        }
    }
}

