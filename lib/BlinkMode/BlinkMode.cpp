#include "BlinkMode.hpp"
#include "Config.h"
#include <Arduino.h>

BlinkMode::BlinkMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService,
                  "Random Blink",
                  "Random blinking pattern with white LEDs",
                  "Friedjof",
                  "v1.0.0") {
}

void BlinkMode::setup() {
    Serial.println("[INFO] BlinkMode setup - Random white blinking");
    lightService->setup();
    lightService->setColor(CRGB::White);
    lightService->setBrightness(255);
    
    // Set effect duration for BlinkMode
    effectDuration = 4000; // 4 seconds
    
    currentInterval = normalInterval;
    lastUpdate = millis();
    
    // Initialize some LEDs as on for visibility
    for (int i = 0; i < NUM_LEDS; i++) {
        ledStates[i] = (i % 2 == 0) ? 255 : 0; // Start with alternating pattern
        if (ledStates[i] > 0) {
            lightService->setLedColor(i, CRGB::White);
        } else {
            lightService->setLedColor(i, CRGB::Black);
        }
    }
}

void BlinkMode::donationTriggered() {
    Serial.println("[INFO] BlinkMode donation triggered - Rapid blinking!");
    
    // Start donation effect
    startDonationEffect();
    
    // Speed up blinking
    currentInterval = fastInterval;
    
    // Play sound
    speakerService->playSound("sparkle.mp3");
}

void BlinkMode::loop() {
    unsigned long currentTime = millis();
    
    // Check if donation effect should end
    if (effectActive && currentTime - effectStartTime >= effectDuration) {
        endDonationEffect();
        currentInterval = normalInterval; // Reset to normal speed
        Serial.println("[INFO] BlinkMode donation effect ended - mode will deactivate");
    }
    
    // Blink timing
    if (currentTime - lastUpdate >= currentInterval) {
        lastUpdate = currentTime;
        updateRandomBlinks();
    }
}

void BlinkMode::updateRandomBlinks() {
    // During donation effect, make more LEDs blink
    int numBlinks = effectActive ? NUM_LEDS : 3; // Always blink at least 3 LEDs
    
    for (int i = 0; i < numBlinks; i++) {
        int ledIndex = random(NUM_LEDS);
        
        // Higher chance to toggle during normal operation
        int toggleChance = effectActive ? 70 : 60; // 60-70% chance to toggle
        if (random(100) < toggleChance) {
            ledStates[ledIndex] = (ledStates[ledIndex] == 0) ? 255 : 0;
            
            if (ledStates[ledIndex] > 0) {
                lightService->setLedColor(ledIndex, CRGB::White);
            } else {
                lightService->setLedColor(ledIndex, CRGB::Black);
            }
        }
    }
}
