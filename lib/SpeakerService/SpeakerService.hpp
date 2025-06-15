#ifndef SPEAKER_SERVICE_HPP
#define SPEAKER_SERVICE_HPP

#include <Arduino.h>
#include "Config.h"
#include "DFRobotDFPlayerMini.h"

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
#endif

/**
 * Speaker Service - DFPlayer Mini MP3 Player
 * Based on working DFRobot example code
 */
class SpeakerService {
    private:
#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))
        SoftwareSerial* softSerial;
#endif
        DFRobotDFPlayerMini myDFPlayer;
        bool isInitialized;
        bool isHardwareAvailable;
        uint8_t currentVolume;
        
        void printDetail(uint8_t type, int value);
        
    public:
        SpeakerService();
        ~SpeakerService();

        // Core functionality
        bool setup();
        void loop();
        bool isReady() const;
        
        // High-level audio methods
        void playRandomSound();
        void playSound(const String& soundFile);
        void playDonationSound();
        void playStartupSound();
        
        // Volume control
        void setVolume(uint8_t volume);
        uint8_t getVolume() const;
        void volumeUp();
        void volumeDown();
        
        // Direct track playback
        bool playTrack(uint8_t trackNumber);
        
        // Playback control
        void pause();
        void resume();
        void stop();
        bool next();
        bool previous();
        
        // Status
        bool isPlaying();
        
        // Configuration
        void enableDebug(bool enable = true);
        void reset();
};

#endif // SPEAKER_SERVICE_HPP
