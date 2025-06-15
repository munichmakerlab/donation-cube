#include "SpeakerService.hpp"

SpeakerService::SpeakerService() : 
    isInitialized(false), 
    isHardwareAvailable(false), 
    currentVolume(DFPLAYER_VOLUME)
{
#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))
    softSerial = new SoftwareSerial(DFPLAYER_RX, DFPLAYER_TX);
#endif
}

SpeakerService::~SpeakerService() {
#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))
    if (softSerial) {
        delete softSerial;
        softSerial = nullptr;
    }
#endif
}

bool SpeakerService::setup() {
    if (isInitialized) {
        return isHardwareAvailable;
    }

    // Check boot counter to avoid infinite restart loops
    static int bootAttempts = 0;
    bootAttempts++;

#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Initializing DFPlayer Mini..."));
    Serial.print(F("[SpeakerService] Boot attempt: "));
    Serial.println(bootAttempts);
    Serial.println(F("[SpeakerService] (May take 3~5 seconds)"));
#endif

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))
    if (!softSerial) {
        return false;
    }
    softSerial->begin(DFPLAYER_BAUD_RATE);
    
    // Initialize DFPlayer with software serial - exactly like the example
    if (!myDFPlayer.begin(*softSerial, /*isACK = */true, /*doReset = */true)) {
#if ENABLE_SERIAL_DEBUG
        Serial.println(F("[SpeakerService] Unable to begin:"));
        Serial.println(F("[SpeakerService] 1.Please recheck the connection!"));
        Serial.println(F("[SpeakerService] 2.Please insert the SD card!"));
        
        if (bootAttempts < 3) {
            Serial.println(F("[SpeakerService] Restarting in 10 seconds..."));
        } else {
            Serial.println(F("[SpeakerService] Too many attempts, continuing without DFPlayer..."));
        }
#endif
        
        if (bootAttempts < 3) {
            // Wait for 10 seconds then restart - like the example but with timeout
            unsigned long startTime = millis();
            while(millis() - startTime < 10000) { // 10 second timeout
                delay(0); // Code to compatible with ESP8266 watch dog.
            }
            // Force restart after timeout
            ESP.restart();
        } else {
            // After 3 attempts, continue without DFPlayer
            isInitialized = true;
            isHardwareAvailable = false;
            return false;
        }
    }
    
#elif defined(ESP32)
    // ESP32 hardware serial configuration
    Serial1.begin(DFPLAYER_BAUD_RATE, SERIAL_8N1, DFPLAYER_RX, DFPLAYER_TX);
    
    // Initialize DFPlayer with hardware serial - exactly like the example
    if (!myDFPlayer.begin(Serial1, /*isACK = */true, /*doReset = */true)) {
#if ENABLE_SERIAL_DEBUG
        Serial.println(F("[SpeakerService] Unable to begin:"));
        Serial.println(F("[SpeakerService] 1.Please recheck the connection!"));
        Serial.println(F("[SpeakerService] 2.Please insert the SD card!"));
        
        if (bootAttempts < 3) {
            Serial.println(F("[SpeakerService] Restarting in 10 seconds..."));
        } else {
            Serial.println(F("[SpeakerService] Too many attempts, continuing without DFPlayer..."));
        }
#endif
        
        if (bootAttempts < 3) {
            // Wait for 10 seconds then restart - like the example but with timeout
            unsigned long startTime = millis();
            while(millis() - startTime < 10000) { // 10 second timeout
                delay(0); // Code to compatible with ESP32 watch dog.
            }
            // Force restart after timeout
            ESP.restart();
        } else {
            // After 3 attempts, continue without DFPlayer
            isInitialized = true;
            isHardwareAvailable = false;
            return false;
        }
    }
    
#else
    // Default hardware serial
    Serial1.begin(DFPLAYER_BAUD_RATE);
    
    if (!myDFPlayer.begin(Serial1, /*isACK = */true, /*doReset = */true)) {
#if ENABLE_SERIAL_DEBUG
        Serial.println(F("[SpeakerService] Unable to begin:"));
        Serial.println(F("[SpeakerService] 1.Please recheck the connection!"));
        Serial.println(F("[SpeakerService] 2.Please insert the SD card!"));
        
        if (bootAttempts < 3) {
            Serial.println(F("[SpeakerService] Restarting in 10 seconds..."));
        } else {
            Serial.println(F("[SpeakerService] Too many attempts, continuing without DFPlayer..."));
        }
#endif
        
        if (bootAttempts < 3) {
            // Wait for 10 seconds then restart - like the example but with timeout
            unsigned long startTime = millis();
            while(millis() - startTime < 10000) { // 10 second timeout
                delay(0); // Code to compatible with watch dog.
            }
            // Force restart after timeout
            ESP.restart();
        } else {
            // After 3 attempts, continue without DFPlayer
            isInitialized = true;
            isHardwareAvailable = false;
            return false;
        }
    }
#endif

    isInitialized = true;
    isHardwareAvailable = true;

#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] DFPlayer Mini online."));
#endif

    // Set default volume - like in the example
    setVolume(currentVolume);
    
    return true;

    this->playDonationSound(); // Play a random donation sound on startup
}

void SpeakerService::loop() {
    if (!isReady()) {
        return;
    }
    
    // Check for DFPlayer messages/errors
    if (myDFPlayer.available()) {
        printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
}

bool SpeakerService::isReady() const {
    return isInitialized && isHardwareAvailable;
}

void SpeakerService::playRandomSound() {
    if (!isReady()) {
        return;
    }
    
    // Generate random track number between DONATION_SOUND_BASE and DONATION_SOUND_BASE + DONATION_SOUND_COUNT - 1
    uint8_t randomTrack = DONATION_SOUND_BASE + random(DONATION_SOUND_COUNT);
    playTrack(randomTrack);
    
#if ENABLE_SERIAL_DEBUG
    Serial.print(F("[SpeakerService] Playing random sound: "));
    Serial.println(randomTrack);
#endif
}

void SpeakerService::playSound(const String& soundFile) {
    if (!isReady()) {
        return;
    }
    
    // Extract track number from filename (e.g., "001.mp3" -> 1)
    int trackNumber = soundFile.toInt();
    if (trackNumber > 0) {
        playTrack(trackNumber);
    }
    
#if ENABLE_SERIAL_DEBUG
    Serial.print(F("[SpeakerService] Playing sound file: "));
    Serial.println(soundFile);
#endif
}

void SpeakerService::playDonationSound() {
    playRandomSound();
}

void SpeakerService::playStartupSound() {
    if (!isReady()) {
        return;
    }
    
    playTrack(STARTUP_SOUND_FILE);
    
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Playing startup sound"));
#endif
}

void SpeakerService::setVolume(uint8_t volume) {
    if (!isReady()) {
        return;
    }
    
    // Constrain volume to valid range (0-30 for DFPlayer Mini)
    volume = constrain(volume, 0, 30);
    currentVolume = volume;
    myDFPlayer.volume(volume);
    
#if ENABLE_SERIAL_DEBUG
    Serial.print(F("[SpeakerService] Volume set to: "));
    Serial.println(volume);
#endif
}

uint8_t SpeakerService::getVolume() const {
    return currentVolume;
}

void SpeakerService::volumeUp() {
    if (currentVolume < 30) {
        setVolume(currentVolume + 1);
    }
}

void SpeakerService::volumeDown() {
    if (currentVolume > 0) {
        setVolume(currentVolume - 1);
    }
}

bool SpeakerService::playTrack(uint8_t trackNumber) {
    if (!isReady()) {
        return false;
    }
    
    myDFPlayer.play(trackNumber);
    
#if ENABLE_SERIAL_DEBUG
    Serial.print(F("[SpeakerService] Playing track: "));
    Serial.println(trackNumber);
#endif
    
    return true;
}

void SpeakerService::pause() {
    if (!isReady()) {
        return;
    }
    
    myDFPlayer.pause();
    
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Paused"));
#endif
}

void SpeakerService::resume() {
    if (!isReady()) {
        return;
    }
    
    myDFPlayer.start();
    
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Resumed"));
#endif
}

void SpeakerService::stop() {
    if (!isReady()) {
        return;
    }
    
    myDFPlayer.stop();
    
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Stopped"));
#endif
}

bool SpeakerService::next() {
    if (!isReady()) {
        return false;
    }
    
    myDFPlayer.next();
    
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Next track"));
#endif
    
    return true;
}

bool SpeakerService::previous() {
    if (!isReady()) {
        return false;
    }
    
    myDFPlayer.previous();
    
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Previous track"));
#endif
    
    return true;
}

bool SpeakerService::isPlaying() {
    if (!isReady()) {
        return false;
    }
    
    // Note: DFPlayer Mini doesn't have a direct "isPlaying" method
    // This would need to be tracked based on play/stop events
    // For now, we return false as a safe default
    return false;
}

void SpeakerService::enableDebug(bool enable) {
    // Debug is controlled by ENABLE_SERIAL_DEBUG in Config.h
    // This method could be used for runtime debug control if needed
    (void)enable; // Suppress unused parameter warning
}

void SpeakerService::reset() {
    if (!isReady()) {
        return;
    }
    
    myDFPlayer.reset();
    
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("[SpeakerService] Reset"));
#endif
    
    // Give some time for reset to complete
    delay(1000);
    
    // Restore volume after reset
    setVolume(currentVolume);
}

void SpeakerService::printDetail(uint8_t type, int value) {
#if ENABLE_SERIAL_DEBUG
    switch (type) {
        case TimeOut:
            Serial.println(F("[DFPlayer] Time Out!"));
            break;
        case WrongStack:
            Serial.println(F("[DFPlayer] Stack Wrong!"));
            break;
        case DFPlayerCardInserted:
            Serial.println(F("[DFPlayer] Card Inserted!"));
            break;
        case DFPlayerCardRemoved:
            Serial.println(F("[DFPlayer] Card Removed!"));
            break;
        case DFPlayerCardOnline:
            Serial.println(F("[DFPlayer] Card Online!"));
            break;
        case DFPlayerUSBInserted:
            Serial.println(F("[DFPlayer] USB Inserted!"));
            break;
        case DFPlayerUSBRemoved:
            Serial.println(F("[DFPlayer] USB Removed!"));
            break;
        case DFPlayerPlayFinished:
            Serial.print(F("[DFPlayer] Track "));
            Serial.print(value);
            Serial.println(F(" finished playing"));
            break;
        case DFPlayerError:
            Serial.print(F("[DFPlayer] Error: "));
            switch (value) {
                case Busy:
                    Serial.println(F("Card not found"));
                    break;
                case Sleeping:
                    Serial.println(F("Sleeping"));
                    break;
                case SerialWrongStack:
                    Serial.println(F("Get Wrong Stack"));
                    break;
                case CheckSumNotMatch:
                    Serial.println(F("Check Sum Not Match"));
                    break;
                case FileIndexOut:
                    Serial.println(F("File Index Out of Bound"));
                    break;
                case FileMismatch:
                    Serial.println(F("Cannot Find File"));
                    break;
                case Advertise:
                    Serial.println(F("In Advertise"));
                    break;
                default:
                    Serial.print(F("Unknown error code: "));
                    Serial.println(value);
                    break;
            }
            break;
        default:
            Serial.print(F("[DFPlayer] Unknown message type: "));
            Serial.print(type);
            Serial.print(F(", value: "));
            Serial.println(value);
            break;
    }
#else
    // Suppress unused parameter warnings when debug is disabled
    (void)type;
    (void)value;
#endif
}
