#include "WaveMode.hpp"
#include "Config.h"
#include <Arduino.h>

WaveMode::WaveMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService,
                  "Wave Motion",
                  "Wave effect moving through LED strip",
                  "Friedjof",
                  "v1.0.0") {
}

void WaveMode::setup() {
    Serial.println("[INFO] WaveMode setup - White wave effect");
    lightService->setup();
    lightService->setColor(CRGB::White);
    lightService->setBrightness(255); // Set proper brightness
    
    // Set effect duration for WaveMode
    effectDuration = 3000; // 3 seconds
    
    wavePosition = 0;
    currentSpeed = normalSpeed;
    lastUpdate = millis();
    
    // Initial wave setup
    updateWave();
}

void WaveMode::donationTriggered() {
    Serial.println("[INFO] WaveMode donation triggered - Waves accelerate!");
    
    // Start donation effect
    startDonationEffect();
    
    // Speed up wave movement
    currentSpeed = fastSpeed;
    
    // Play sound
    speakerService->playDonationSound();
}

void WaveMode::loop() {
    unsigned long currentTime = millis();
    
    // Check if donation effect should end
    if (effectActive && currentTime - effectStartTime >= effectDuration) {
        endDonationEffect();
        currentSpeed = normalSpeed; // Reset to normal speed
        Serial.println("[INFO] WaveMode donation effect ended - mode will deactivate");
    }
    
    // Wave movement timing
    if (currentTime - lastUpdate >= currentSpeed) {
        lastUpdate = currentTime;
        
        // Move wave position
        wavePosition++;
        if (wavePosition >= NUM_LEDS) {
            wavePosition = 0;
        }
        
        updateWave();
    }
}

void WaveMode::updateWave() {
    // First set all LEDs to dim white for base lighting
    for (int i = 0; i < NUM_LEDS; i++) {
        CRGB dimWhite = CRGB::White;
        dimWhite.fadeToBlackBy(200); // Very dim base lighting
        lightService->setLedColor(i, dimWhite);
    }
    
    // Set wave LEDs to bright white
    for (int i = 0; i < waveWidth; i++) {
        int ledIndex = (wavePosition + i) % NUM_LEDS;
        
        // Create gradient within wave
        uint8_t brightness = 255 - (i * 60); // Less aggressive fade
        if (effectActive) {
            brightness = 255; // Full brightness during donation
        }
        
        CRGB color = CRGB::White;
        color.fadeToBlackBy(255 - brightness);
        lightService->setLedColor(ledIndex, color);
    }
}
