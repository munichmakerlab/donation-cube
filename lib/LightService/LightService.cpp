#include "LightService.hpp"

LightService::LightService() 
    : currentBrightness(MIN_BRIGHTNESS), newBrightness(MIN_BRIGHTNESS) {
}

void LightService::setup() {
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(currentBrightness);
    FastLED.clear();
    
    // Set initial white color
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
    }
    
    FastLED.show();
    Serial.println("[INFO] LightService initialized");
}

void LightService::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    FastLED.setBrightness(currentBrightness);
    FastLED.show();
}

void LightService::setColor(const CRGB& color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
    FastLED.show();
}

void LightService::setLedColor(uint8_t index, const CRGB& color) {
    if (index < NUM_LEDS) {
        leds[index] = color;
        FastLED.show();
    }
}
