#include "SensorService.hpp"

void SensorService::setup() {
    pinMode(sensorPin, INPUT_PULLUP);
    sensorState = digitalRead(sensorPin);
    lastSensorState = sensorState;
    risingEdgeDetected = false;
    fallingEdgeDetected = false;
    Serial.println("[INFO] SensorService initialized");
}

void SensorService::loop() {
    // Update sensor state and detect edges
    lastSensorState = sensorState;
    sensorState = digitalRead(sensorPin);
    
    // Detect rising edge (HIGH to LOW for TCRT5000 - donation detected)
    if (lastSensorState == HIGH && sensorState == LOW) {
        risingEdgeDetected = true;
        Serial.println("[SENSOR] Rising edge detected - donation placed");
    }
    
    // Detect falling edge (LOW to HIGH for TCRT5000 - donation removed)
    if (lastSensorState == LOW && sensorState == HIGH) {
        fallingEdgeDetected = true;
        Serial.println("[SENSOR] Falling edge detected - donation removed");
    }
}

bool SensorService::risingEdge() {
    if (risingEdgeDetected) {
        risingEdgeDetected = false; // Reset flag after reading
        return true;
    }
    return false;
}

bool SensorService::fallingEdge() {
    if (fallingEdgeDetected) {
        fallingEdgeDetected = false; // Reset flag after reading
        return true;
    }
    return false;
}

bool SensorService::isActive() {
    return digitalRead(sensorPin) == LOW; // LOW means donation detected
}
