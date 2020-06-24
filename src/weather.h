#include <Arduino.h>
#include <ESP8266HTTPClient.h>

extern const String weatherServername;
extern const String cityID;
extern const String apiKey;
extern int32_t lastWeatherQuery;
extern String weatherResult;
extern WiFiClient weatherClient;

extern String weatherDescription;
extern String weatherLocation;
extern String country;
extern float temperature;
extern float humidity;
extern float pressure;

bool handleWeatherData();
void updateWeatherData();