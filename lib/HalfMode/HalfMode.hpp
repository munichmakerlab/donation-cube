#ifndef HALF_MODE_HPP
#define HALF_MODE_HPP

#include "AbstractMode.hpp"

class HalfMode : public AbstractMode {
    private:
        unsigned long lastUpdate = 0;
        bool showFirstHalf = true;
        unsigned long normalInterval = 1500; // Switch every 1.5 seconds
        unsigned long fastInterval = 300;    // Fast switching during donation
        unsigned long currentInterval = 1500;
        
    public:
        HalfMode(LightService* lightService, SpeakerService* speakerService);
        
        void donationTriggered() override;
        void loop() override;
        void setup() override;
        
    private:
        void updateHalves();
};

#endif // HALF_MODE_HPP
