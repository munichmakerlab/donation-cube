#ifndef LIGHT_SERVICE_HPP
#define LIGHT_SERVICE_HPP

#include <Arduino.h>
#include <FastLED.h>

#include "Config.h"

class LightService {
    private:
        CRGB leds[NUM_LEDS];
        const uint8_t numLeds = NUM_LEDS;

        uint8_t currentBrightness;
        uint8_t newBrightness;

    public:
        LightService();

        void setBrightness(uint8_t brightness);
        void setColor(const CRGB& color);
        void setLedColor(uint8_t index, const CRGB& color);
        
        uint8_t getBrightness() const { return currentBrightness; }
        void show() { FastLED.show(); }
        void clear() { FastLED.clear(); }

        void setup();
};

#endif // LIGHT_SERVICE_HPP