#ifndef SPEAKER_SERVICE_HPP
#define SPEAKER_SERVICE_HPP

#include <Arduino.h>

#include "Config.h"

class SpeakerService {
    private:
        // Pin for the speaker
        // …

    public:
        SpeakerService();

        void playRandomSound();
        void playSound(const String& soundFile);

        void setup();
};

#endif // SPEAKER_SERVICE_HPP