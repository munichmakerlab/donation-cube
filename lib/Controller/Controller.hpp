#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <Arduino.h>

#include "AbstractMode.hpp"
#include "SensorService.hpp"
#include "SpeakerService.hpp"

#define MAX_MODES 10  // Maximum number of modes

class Controller {
    private:
        AbstractMode* modes[MAX_MODES];
        uint8_t modeCount = 0;
        uint8_t currentModeIndex = 0;

        uint64_t lastSensorCheck = 0; // Track last mode switch time

        SensorService* sensorService;
        SpeakerService* speakerService;

        void switchMode(uint8_t index);
        void switchNextMode();
        
    public:
        Controller(SensorService* sensorService, SpeakerService* speakerService)
            : sensorService(sensorService), speakerService(speakerService) {}

        void addMode(AbstractMode *mode);
        void switchToNextMode(); // Public method to manually switch modes

        void setup();
        void loop();
        
        // Public getters for MQTT integration
        String getCurrentModeName() const;
        uint8_t getModeCount() const { return modeCount; }
        uint8_t getCurrentModeIndex() const { return currentModeIndex; }
};

#endif // CONTROLLER_HPP