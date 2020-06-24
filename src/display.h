#include <FastLED.h>

#define ledQuantity 150
#define ledRows 7
#define ledColumns 22
#define dataPin 2

extern CRGB leds[ledQuantity];
extern CRGB* display[ledRows][ledColumns];
extern int lastTimeShown;

void testLed();
void setupLED();
void writeDigit(int number, int row, int column, long color);
void writeNumber(unsigned int number, long color);
void writeTime(int hours, int minutes, long color);
void writeTemp(int temperature, long color);
void testAllLeds();