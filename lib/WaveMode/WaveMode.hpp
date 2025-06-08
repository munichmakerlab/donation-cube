#ifndef WAVE_MODE_HPP
#define WAVE_MODE_HPP

#include "AbstractMode.hpp"

class WaveMode : public AbstractMode {
    private:
        uint8_t wavePosition = 0;
        uint8_t waveWidth = 2; // Width of the wave
        unsigned long lastUpdate = 0;
        unsigned long normalSpeed = 200; // Normal wave speed
        unsigned long fastSpeed = 50;    // Fast wave speed during donation
        unsigned long currentSpeed = 200;
        
    public:
        WaveMode(LightService* lightService, SpeakerService* speakerService);
        
        void donationTriggered() override;
        void loop() override;
        void setup() override;
        
    private:
        void updateWave();
};

#endif // WAVE_MODE_HPP
