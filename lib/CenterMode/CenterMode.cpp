#include "CenterMode.hpp"
#include "Config.h"
#include <Arduino.h>

CenterMode::CenterMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService, 3000,
                  "Center Expansion",
                  "Light expanding from center outwards",
                  "Friedjof",
                  "v1.0.0") { // 3 second donation effect
    maxRadius = NUM_LEDS / 2; // Maximum expansion radius
}

void CenterMode::setup() {
    Serial.println("[CenterMode] Initializing center expansion mode");
    lightService->clear();
    currentRadius = 0;
    expanding = true;
    currentInterval = normalInterval;
    lastUpdate = millis();
}

void CenterMode::donationTriggered() {
    Serial.println("[CenterMode] Donation detected - starting fast expansion effect");
    startDonationEffect();
    
    // Play donation sound
    speakerService->playDonationSound();
    
    // Speed up expansion during donation
    currentInterval = fastInterval;
    
    // Reset expansion from center
    currentRadius = 0;
    expanding = true;
}

void CenterMode::loop() {
    unsigned long now = millis();
    
    // Check if donation effect should end
    if (isDonationEffectActive() && now - getDonationStartTime() >= getEffectDuration()) {
        endDonationEffect();
        currentInterval = normalInterval; // Return to normal speed
        return;
    }
    
    // Update expansion animation
    if (now - lastUpdate >= currentInterval) {
        updateExpansion();
        lastUpdate = now;
    }
}

void CenterMode::updateExpansion() {
    lightService->clear();
    
    if (expanding) {
        // Expand from center
        currentRadius++;
        if (currentRadius >= maxRadius) {
            expanding = false;
            currentRadius = maxRadius;
        }
    } else {
        // Contract to center
        currentRadius--;
        if (currentRadius <= 0) {
            expanding = true;
            currentRadius = 0;
        }
    }
    
    setRadiusLEDs(currentRadius);
    lightService->show();
}

void CenterMode::setRadiusLEDs(int radius) {
    if (radius <= 0) {
        // Only center LED(s)
        int center = NUM_LEDS / 2;
        lightService->setLedColor(center, CRGB(255, 255, 255));
        if (NUM_LEDS % 2 == 0) {
            // Even number of LEDs, light both center LEDs
            lightService->setLedColor(center - 1, CRGB(255, 255, 255));
        }
        return;
    }
    
    int center = NUM_LEDS / 2;
    
    // Light LEDs in the current radius from center
    for (int i = 0; i < radius && i < maxRadius; i++) {
        // Calculate brightness - brighter at center, dimmer at edges
        int brightness = map(i, 0, radius, 255, 100);
        brightness = constrain(brightness, 50, 255);
        
        // Light LEDs on both sides of center
        int leftPos = center - i - 1;
        int rightPos = center + i;
        
        if (NUM_LEDS % 2 == 0) {
            // Even number of LEDs
            leftPos = center - i - 1;
            rightPos = center + i;
        } else {
            // Odd number of LEDs, center is at NUM_LEDS/2
            if (i == 0) {
                // Light center LED
                lightService->setLedColor(center, CRGB(brightness, brightness, brightness));
                continue;
            }
            leftPos = center - i;
            rightPos = center + i;
        }
        
        // Set LEDs if they're within bounds
        if (leftPos >= 0) {
            lightService->setLedColor(leftPos, CRGB(brightness, brightness, brightness));
        }
        if (rightPos < NUM_LEDS) {
            lightService->setLedColor(rightPos, CRGB(brightness, brightness, brightness));
        }
    }
}
