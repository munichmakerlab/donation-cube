#ifndef STATIC_MODE_HPP
#define STATIC_MODE_HPP

#include "AbstractMode.hpp"

class StaticMode : public AbstractMode {
    private:
        uint8_t currentBrightness = MIN_BRIGHTNESS;
        uint8_t newBrightness = MIN_BRIGHTNESS;
        uint8_t direction = 1; // 1 = increasing, 0 = decreasing
        unsigned long lastUpdate = 0;
        unsigned long speed = BREATH_SPEED_NORMAL;
        
    public:
        StaticMode(LightService* lightService, SpeakerService* speakerService);
        
        void donationTriggered() override;
        void loop() override;
        void setup() override;
};

#endif // STATIC_MODE_HPP