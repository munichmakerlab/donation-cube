#ifndef BLINK_MODE_HPP
#define BLINK_MODE_HPP

#include "AbstractMode.hpp"

class BlinkMode : public AbstractMode {
    private:
        unsigned long lastUpdate = 0;
        unsigned long normalInterval = 300; // Normal blink interval
        unsigned long fastInterval = 100;   // Fast blink interval during donation
        unsigned long currentInterval = 300;
        
        uint8_t ledStates[NUM_LEDS]; // 0 = off, 255 = on
        
    public:
        BlinkMode(LightService* lightService, SpeakerService* speakerService);
        
        void donationTriggered() override;
        void loop() override;
        void setup() override;
        
    private:
        void updateRandomBlinks();
};

#endif // BLINK_MODE_HPP
