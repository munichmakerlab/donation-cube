#include "Controller.hpp"

void Controller::addMode(AbstractMode *mode) {
    if (modeCount < MAX_MODES) {
        modes[modeCount] = mode;
        modeCount++;
    } else {
        Serial.println("[ERROR] Maximum number of modes reached!");
    }
}

void Controller::switchToNextMode() {
    switchNextMode();
}

void Controller::switchMode(uint8_t index) {
    if (index < modeCount) {
        // Deactivate current mode
        if (modeCount > 0) {
            modes[currentModeIndex]->deactivate();
        }
        
        currentModeIndex = index;
        
        // Activate new mode
        modes[currentModeIndex]->activate();
        modes[currentModeIndex]->printModeInfo();
        Serial.print("[INFO] Switched to mode ");
        Serial.println(index);
    } else {
        Serial.println("[ERROR] Invalid mode index");
    }
}

void Controller::switchNextMode() {
    uint8_t nextIndex = (currentModeIndex + 1) % modeCount;
    switchMode(nextIndex);
}

void Controller::setup() {
    Serial.println("[INFO] Controller setup started");
    
    if (modeCount > 0) {
        // Activate the first mode
        modes[0]->activate();
        modes[0]->printModeInfo();
        Serial.print("[INFO] Activated initial mode with ");
        Serial.print(modeCount);
        Serial.println(" total modes");
    } else {
        Serial.println("[WARNING] No modes registered");
    }
}

void Controller::loop() {
    if (modeCount == 0) return;

    // Check for sensor state changes
    if (sensorService->risingEdge()) {
        modes[currentModeIndex]->donationTriggered();
    }

    if (modes[currentModeIndex]->isActive()) {
        modes[currentModeIndex]->loop();
    } else {
        switchNextMode();
    }
}