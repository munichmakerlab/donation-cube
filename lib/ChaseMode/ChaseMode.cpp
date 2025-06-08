#include "ChaseMode.hpp"
#include "Config.h"
#include <Arduino.h>

ChaseMode::ChaseMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService, 2500,
                  "Chase Light",
                  "Moving light with trailing tail effect",
                  "Friedjof",
                  "v1.0.0") { // 2.5 second donation effect
}

void ChaseMode::setup() {
    Serial.println("[ChaseMode] Initializing chase light mode");
    lightService->clear();
    currentPosition = 0;
    direction = 1;
    currentInterval = normalInterval;
    lastUpdate = millis();
}

void ChaseMode::donationTriggered() {
    Serial.println("[ChaseMode] Donation detected - starting fast chase effect");
    startDonationEffect();
    
    // Speed up chase during donation
    currentInterval = fastInterval;
    
    // Optionally change direction for visual effect
    direction *= -1;
}

void ChaseMode::loop() {
    unsigned long now = millis();
    
    // Check if donation effect should end
    if (isDonationEffectActive() && now - getDonationStartTime() >= getEffectDuration()) {
        endDonationEffect();
        currentInterval = normalInterval; // Return to normal speed
        return;
    }
    
    // Update chase animation
    if (now - lastUpdate >= currentInterval) {
        updateChase();
        lastUpdate = now;
    }
}

void ChaseMode::updateChase() {
    // Move the chaser
    currentPosition += direction;
    
    // Bounce at the ends
    if (currentPosition >= NUM_LEDS) {
        currentPosition = NUM_LEDS - 1;
        direction = -1;
    } else if (currentPosition < 0) {
        currentPosition = 0;
        direction = 1;
    }
    
    drawChaser();
    lightService->show();
}

void ChaseMode::drawChaser() {
    lightService->clear();
    
    // Draw the main chaser LED (brightest)
    lightService->setLedColor(currentPosition, CRGB(255, 255, 255));
    
    // Draw the tail behind the chaser
    for (int i = 1; i <= tailLength; i++) {
        int tailPos = currentPosition - (direction * i);
        
        // Make sure tail position is within bounds
        if (tailPos >= 0 && tailPos < NUM_LEDS) {
            // Calculate brightness for tail (dimmer as it gets further back)
            int brightness = map(i, 1, tailLength, 180, 30);
            brightness = constrain(brightness, 30, 180);
            
            lightService->setLedColor(tailPos, CRGB(brightness, brightness, brightness));
        }
    }
    
    // Optional: Add a small preview LED ahead of the chaser during donation effect
    if (isDonationEffectActive()) {
        int previewPos = currentPosition + direction;
        if (previewPos >= 0 && previewPos < NUM_LEDS) {
            lightService->setLedColor(previewPos, CRGB(80, 80, 80));
        }
    }
}
