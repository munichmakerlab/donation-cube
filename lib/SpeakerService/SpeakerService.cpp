#include "SpeakerService.hpp"

SpeakerService::SpeakerService() {
}

void SpeakerService::setup() {
    // Mock implementation - no actual hardware setup needed
    Serial.println("[INFO] SpeakerService initialized (Mock)");
}

void SpeakerService::playRandomSound() {
    // Mock implementation - just print to serial
    Serial.println("[MOCK] Playing random sound effect");
}

void SpeakerService::playSound(const String& soundFile) {
    // Mock implementation - just print to serial
    Serial.print("[MOCK] Playing sound file: ");
    Serial.println(soundFile);
}
