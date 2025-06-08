#ifndef CHASE_MODE_HPP
#define CHASE_MODE_HPP

#include "AbstractMode.hpp"

class ChaseMode : public AbstractMode {
    private:
        unsigned long lastUpdate = 0;
        int currentPosition = 0;
        int direction = 1; // 1 for forward, -1 for backward
        int tailLength = 3; // Number of LEDs in the tail
        unsigned long normalInterval = 120; // Normal chase speed
        unsigned long fastInterval = 40;    // Fast chase during donation
        unsigned long currentInterval = 120;
        
    public:
        ChaseMode(LightService* lightService, SpeakerService* speakerService);
        
        void donationTriggered() override;
        void loop() override;
        void setup() override;
        
    private:
        void updateChase();
        void drawChaser();
};

#endif // CHASE_MODE_HPP
