#include <NeoPixelBus.h>

#define ledQuantity 150
#define ledRows 7
#define ledColumns 22
#define dataPin 2

extern NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart1800KbpsMethod> strip;
extern int lastTimeShown;

int mapToStrip(int row, int col);
void setupLED();
void writeDigit(int number, int row, int column, long color);
void writeNumber(unsigned int number, long color);
void writeTime(int hours, int minutes, long color);
void writeTemp(int temperature, long color);
void testAllLeds();