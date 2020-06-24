#include <FastLED.h>
#include "display.h"

extern CRGB leds[ledQuantity];
extern CRGB* display[ledRows][ledColumns];

void rainbowbg(float speed, int brightness);
void staticbg(long color);