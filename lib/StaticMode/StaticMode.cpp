#include "StaticMode.hpp"
#include "Config.h"
#include <Arduino.h>

StaticMode::StaticMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService, 
                  "Static Breathing", 
                  "Gentle breathing effect with white LEDs", 
                  "Friedjof", 
                  "v1.0.0") {
}

void StaticMode::setup() {
    Serial.println("[INFO] StaticMode setup");
    lightService->setup();
    lightService->setColor(CRGB::White);
    lightService->setBrightness(MIN_BRIGHTNESS);
    
    // Set effect duration for StaticMode
    effectDuration = 3000; // 3 seconds
    
    currentBrightness = MIN_BRIGHTNESS;
    newBrightness = MIN_BRIGHTNESS;
    direction = 1;
    speed = BREATH_SPEED_NORMAL;
    lastUpdate = millis();
}

void StaticMode::donationTriggered() {
    Serial.println("[INFO] StaticMode donation triggered!");
    
    // Start donation effect
    startDonationEffect();
    
    // Speed up breathing effect
    speed = BREATH_SPEED_FAST;
    
    // Play sound
    speakerService->playRandomSound();
}

void StaticMode::loop() {
    unsigned long currentTime = millis();
    
    // Check if donation effect should end
    if (effectActive && currentTime - effectStartTime >= effectDuration) {
        endDonationEffect();
        speed = BREATH_SPEED_NORMAL;
        Serial.println("[INFO] StaticMode donation effect ended - mode will deactivate");
    }
    
    // Breathing effect timing
    if (currentTime - lastUpdate >= speed) {
        lastUpdate = currentTime;
        
        // Update brightness direction
        if (direction) {
            if (newBrightness + BRIGHTNESS_STEP >= MAX_BRIGHTNESS) {
                newBrightness = MAX_BRIGHTNESS;
                direction = 0;
            } else {
                newBrightness += BRIGHTNESS_STEP;
            }
        } else {
            if (newBrightness - BRIGHTNESS_STEP <= MIN_BRIGHTNESS) {
                newBrightness = MIN_BRIGHTNESS;
                direction = 1;
            } else {
                newBrightness -= BRIGHTNESS_STEP;
            }
        }
        
        // Apply brightness change
        if (newBrightness != currentBrightness) {
            currentBrightness = newBrightness;
            lightService->setBrightness(currentBrightness);
        }
    }
}
