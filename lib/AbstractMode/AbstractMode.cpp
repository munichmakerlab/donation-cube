#include "AbstractMode.hpp"

void AbstractMode::startDonationEffect() {
    effectActive = true;
    effectStartTime = millis();
}

void AbstractMode::endDonationEffect() {
    effectActive = false;
    effectStartTime = 0;
    // Deactivate mode after effect ends to trigger mode switch
    deactivate();
}

bool AbstractMode::isActive() {
    return active;
}

void AbstractMode::activate() {
    active = true;
    setup();
}

void AbstractMode::deactivate() {
    active = false;
}

void AbstractMode::printModeInfo() const {
    Serial.println("========================================");
    Serial.println("           MODE INFORMATION");
    Serial.println("========================================");
    Serial.print("Name:        "); Serial.println(name);
    Serial.print("Description: "); Serial.println(description);
    Serial.print("Author:      "); Serial.println(author);
    Serial.print("Version:     "); Serial.println(version);
    Serial.println("========================================");
}
