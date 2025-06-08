#include "HalfMode.hpp"
#include "Config.h"
#include <Arduino.h>

HalfMode::HalfMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService,
                  "Half Switch",
                  "Alternating first and second half illumination",
                  "Friedjof",
                  "v1.0.0") {
}

void HalfMode::setup() {
    Serial.println("[INFO] HalfMode setup - First/Second half alternating");
    lightService->setup();
    lightService->setBrightness(255);
    
    // Set effect duration for HalfMode
    effectDuration = 3000; // 3 seconds
    
    showFirstHalf = true;
    currentInterval = normalInterval;
    lastUpdate = millis();
    
    // Start with first half
    updateHalves();
}

void HalfMode::donationTriggered() {
    Serial.println("[INFO] HalfMode donation triggered - Rapid half switching!");
    
    // Start donation effect
    startDonationEffect();
    
    // Speed up switching
    currentInterval = fastInterval;
    
    // Play sound
    speakerService->playSound("switch.mp3");
}

void HalfMode::loop() {
    unsigned long currentTime = millis();
    
    // Check if donation effect should end
    if (effectActive && currentTime - effectStartTime >= effectDuration) {
        endDonationEffect();
        currentInterval = normalInterval; // Reset to normal speed
        Serial.println("[INFO] HalfMode donation effect ended - mode will deactivate");
    }
    
    // Switch between halves timing
    if (currentTime - lastUpdate >= currentInterval) {
        lastUpdate = currentTime;
        showFirstHalf = !showFirstHalf;
        updateHalves();
    }
}

void HalfMode::updateHalves() {
    // Clear all LEDs first
    for (int i = 0; i < NUM_LEDS; i++) {
        lightService->setLedColor(i, CRGB::Black);
    }
    
    int halfPoint = NUM_LEDS / 2;
    
    if (showFirstHalf) {
        // Light up first half
        for (int i = 0; i < halfPoint; i++) {
            lightService->setLedColor(i, CRGB::White);
        }
    } else {
        // Light up second half
        for (int i = halfPoint; i < NUM_LEDS; i++) {
            lightService->setLedColor(i, CRGB::White);
        }
    }
}
