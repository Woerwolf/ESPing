#include "display.h"

#include "OTA.h"
#include <FastLED.h>

// #define DEBUGGING 1

// this function gets the position of the desired led as input (row & column) and returns the index of it on the strip
int mapToStrip(int row, int col){
    const int amountOfCompleteColumns = 16;

    // set short columns on the left (4,5,6)
    // left column with 4 leds
    if(col == 0){
        if(row == 3) return 1;
        if(row == 4) return 0;
        if(row == 5) return 8;
        if(row == 6) return 9;
    }

    // left column with 5 leds
    if(col == 1){
        if(row == 2) return 2;
        if(row == 3) return 6;
        if(row == 4) return 7;
        if(row == 5) return 11;
        if(row == 6) return 10;
    }

    // left column with 6 leds
    if(col == 2){
        if(row == 1) return 4;
        if(row == 2) return 5;
        if(row == 3) return 13;
        if(row == 4) return 12;
        if(row == 5) return 22;
        if(row == 6) return 23;
    }


    // all columns with 7 leds
    if(col>=3 && col<amountOfCompleteColumns+3){
        if(col%2 == 1){
            if(row == 0) return 3+(7*(col-3));
            if(row == 1) return 15+(7*(col-3));
            if(row == 2) return 14+(7*(col-3));
            if(row == 3) return 20+(7*(col-3));
            if(row == 4) return 21+(7*(col-3));
            if(row == 5) return 25+(7*(col-3));
            if(row == 6) return 24+(7*(col-3));
        }else{
            if(row == 0) return 16+(7*(col-4));
            if(row == 1) return 18+(7*(col-4));
            if(row == 2) return 19+(7*(col-4));
            if(row == 3) return 27+(7*(col-4));
            if(row == 4) return 26+(7*(col-4));
            if(row == 5) return 36+(7*(col-4));
            if(row == 6) return 37+(7*(col-4));
        }
    }

    // right column with 4 leds
    if(col == amountOfCompleteColumns+5){
        if(row == 0) return 129;
        if(row == 1) return 140;
        if(row == 2) return 139;
        if(row == 3) return 141;
    }

    // right column with 5 leds
    if(col == amountOfCompleteColumns+4){
        if(row == 0) return 128;
        if(row == 1) return 130;
        if(row == 2) return 131;
        if(row == 3) return 138;
        if(row == 4) return 137;
    }

    // right column with 6 leds
    if(col == amountOfCompleteColumns+3){
        if(row == 0) return 115;
        if(row == 1) return 127;
        if(row == 2) return 126;
        if(row == 3) return 132;
        if(row == 4) return 133;
        if(row == 5) return 136;
    }

    return -1;
}

void setupLED(){
    strip.Begin();
    strip.Show();
}

// input row and column specify the ball where the number will appear
void writeDigit(int number, int row, int column, long color){
    // check if number is out of range
    if(number > 9 || number < 0){
        #if defined(DEBUGGING)
        Telnet.println("Number out of Range!");
        #endif
        return;
    }

    // check if position is invalid (out of field)
    if(mapToStrip(row-1, column-1) == -1 || mapToStrip(row-1, column+2) == -1 || mapToStrip(row+3, column-1) == -1 || mapToStrip(row+3, column-2) == -1){
        #if defined(DEBUGGING)
        Telnet.println("Invalid place for number!");
        #endif
        return;
    }

    switch (number)
    {
    case 0:
        strip.SetPixelColor(mapToStrip(row, column-1), color);
        strip.SetPixelColor(mapToStrip(row-1, column), color);
        strip.SetPixelColor(mapToStrip(row-1, column+1), color);
        strip.SetPixelColor(mapToStrip(row, column+1), color);
        strip.SetPixelColor(mapToStrip(row+2, column-2), color);
        strip.SetPixelColor(mapToStrip(row+3, column-2), color);
        strip.SetPixelColor(mapToStrip(row+3, column-1), color);
        strip.SetPixelColor(mapToStrip(row+2, column), color);
        break;

    case 2:
        strip.SetPixelColor(mapToStrip(row-1, column), color);
        strip.SetPixelColor(mapToStrip(row-1, column+1), color);
        strip.SetPixelColor(mapToStrip(row, column+1), color);
        strip.SetPixelColor(mapToStrip(row+1, column), color);
        strip.SetPixelColor(mapToStrip(row+1, column-1), color);
        strip.SetPixelColor(mapToStrip(row+2, column-2), color);
        strip.SetPixelColor(mapToStrip(row+3, column-2), color);
        strip.SetPixelColor(mapToStrip(row+3, column-1), color);
        break;

    case 4:
        strip.SetPixelColor(mapToStrip(row-1, column), color);
        strip.SetPixelColor(mapToStrip(row, column-1), color);
        strip.SetPixelColor(mapToStrip(row+1, column-1), color);
        strip.SetPixelColor(mapToStrip(row+1, column), color);
        strip.SetPixelColor(mapToStrip(row, column+1), color);
        strip.SetPixelColor(mapToStrip(row-1, column+2), color);
        strip.SetPixelColor(mapToStrip(row+2, column), color);
        strip.SetPixelColor(mapToStrip(row+3, column-1), color);
        break;

    case 7:
        strip.SetPixelColor(mapToStrip(row-1, column), color);
        strip.SetPixelColor(mapToStrip(row-1, column+1), color);
        strip.SetPixelColor(mapToStrip(row, column+1), color);
        strip.SetPixelColor(mapToStrip(row+1, column), color);
        strip.SetPixelColor(mapToStrip(row+2, column-1), color);
        strip.SetPixelColor(mapToStrip(row+3, column-2), color);
        break;

    case 9:
        strip.SetPixelColor(mapToStrip(row, column-1), color);

    case 3:
        strip.SetPixelColor(mapToStrip(row-1, column), color);
        strip.SetPixelColor(mapToStrip(row+1, column-1), color);
        strip.SetPixelColor(mapToStrip(row+3, column-2), color);

    case 1:
        strip.SetPixelColor(mapToStrip(row-1, column+1), color);
        strip.SetPixelColor(mapToStrip(row, column+1), color);
        strip.SetPixelColor(mapToStrip(row+1, column), color);
        strip.SetPixelColor(mapToStrip(row+2, column), color);
        strip.SetPixelColor(mapToStrip(row+3, column-1), color);
        break;

    case 8:
        strip.SetPixelColor(mapToStrip(row, column+1), color);

    case 6:
        strip.SetPixelColor(mapToStrip(row+2, column-2), color);

    case 5:
        strip.SetPixelColor(mapToStrip(row-1, column+1), color);
        strip.SetPixelColor(mapToStrip(row-1, column), color);
        strip.SetPixelColor(mapToStrip(row, column-1), color);
        strip.SetPixelColor(mapToStrip(row+1, column-1), color);
        strip.SetPixelColor(mapToStrip(row+1, column), color);
        strip.SetPixelColor(mapToStrip(row+2, column), color);
        strip.SetPixelColor(mapToStrip(row+3, column-1), color);
        strip.SetPixelColor(mapToStrip(row+3, column-2), color);
        break;

    default:
        break;
    }

    strip.Show();

}

void writeTime(int hours, int minutes, long color){
    
    // animate blinking dots in the middle
    if(millis()%2000 < 1000){
        strip.SetPixelColor(mapToStrip(4, 10), color);
        strip.SetPixelColor(mapToStrip(2, 11), color);
    }

    //write hours
    if(hours>9){
        writeDigit((int)(hours/10), 2, 4, color);
    }
    writeDigit(hours%10, 2, 8, color);

    //write minutes
    writeDigit((int)(minutes/10), 2, 14, color);
    writeDigit(minutes%10, 2, 18, color);

    strip.Show();

}

void writeNumber(unsigned int number, long color){
    if(number > 99999) return;
    writeDigit(number%10, 2, 19, color);
    if(number>=10){
        writeDigit(int((number%100)/10), 2, 15, color);
    }

    if(number>=100){
        writeDigit(int((number%1000)/100), 2, 11, color);
    }

    if(number>=1000){
        writeDigit(int((number%10000)/1000), 2, 7, color);
    }

    if(number>=10000){
        writeDigit(int((number%100000)/10000), 2, 3, color);
    }

    FastLED.show();
}

void writeTemp(int temperature, long color){
    bool tempNegative;
    if(temperature<0){
        tempNegative = true;
        temperature = temperature*(-1);
    }

    //check if temperature is out of range
    if(temperature>100) return;

    strip.SetPixelColor(mapToStrip(1, 15), color);

    writeDigit(temperature%10, 2, 12, color);
    if(temperature > 10){
        writeDigit(temperature/10, 2, 8, color);
        if(tempNegative){
            strip.SetPixelColor(mapToStrip(3, 5), color);
            strip.SetPixelColor(mapToStrip(3, 4), color);
            return;
        }
    }

    if(tempNegative){
        strip.SetPixelColor(mapToStrip(3, 9), color);
        strip.SetPixelColor(mapToStrip(3, 8), color);
    }
}

void testAllLeds(){
    for(int i = 0; i<ledQuantity; i++){
        strip.SetPixelColor(i, 0x0000ff);
        delay(40);
    }
    for(int i = 0; i<ledQuantity; i++){
        strip.SetPixelColor(i, 0x000000);
        delay(40);
    }   
    #if defined(DEBUGGING)
        Telnet.println("Tested!");
    #endif
}