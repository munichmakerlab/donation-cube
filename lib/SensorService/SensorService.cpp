#include "SensorService.hpp"

void SensorService::setup() {
    pinMode(sensorPin, INPUT_PULLUP);
    sensorState = digitalRead(sensorPin);
    Serial.println("[INFO] SensorService initialized");
}

void SensorService::loop() {
    // Update sensor state - this should be called in main loop
    // to continuously monitor sensor changes
}

bool SensorService::risingEdge() {
    uint8_t newState = digitalRead(sensorPin);
    bool edge = (sensorState == HIGH && newState == LOW); // Donation detected when going from HIGH to LOW
    sensorState = newState;
    return edge;
}

bool SensorService::fallingEdge() {
    uint8_t newState = digitalRead(sensorPin);
    bool edge = (sensorState == LOW && newState == HIGH); // Donation ended when going from LOW to HIGH
    sensorState = newState;
    return edge;
}

bool SensorService::isActive() {
    return digitalRead(sensorPin) == LOW; // LOW means donation detected
}
