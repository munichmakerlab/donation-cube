#ifndef CENTER_MODE_HPP
#define CENTER_MODE_HPP

#include "AbstractMode.hpp"

class CenterMode : public AbstractMode {
    private:
        unsigned long lastUpdate = 0;
        int currentRadius = 0;
        int maxRadius = 0;
        bool expanding = true;
        unsigned long normalInterval = 150; // Normal expansion speed
        unsigned long fastInterval = 50;    // Fast expansion during donation
        unsigned long currentInterval = 150;
        
    public:
        CenterMode(LightService* lightService, SpeakerService* speakerService);
        
        void donationTriggered() override;
        void loop() override;
        void setup() override;
        
    private:
        void updateExpansion();
        void setRadiusLEDs(int radius);
};

#endif // CENTER_MODE_HPP
