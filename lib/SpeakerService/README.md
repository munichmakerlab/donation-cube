# SpeakerService

Professional audio interface for the Donation Box project using DFPlayer Mini MP3 player with robust startup and fallback mechanisms.

## Overview

The SpeakerService provides a production-ready audio system for donation boxes. It features intelligent initialization with retry logic, boot loop protection, and graceful degradation when hardware is unavailable.

## ✨ Key Features

- **🔊 DFPlayer Mini Integration**: Full MP3 playback support
- **🛡️ Robust Startup**: Boot loop protection with 3-attempt limit
- **🔄 Smart Recovery**: Automatic restart on hardware failures
- **🎚️ Production Ready**: Works with/without serial debug
- **🎵 Donation Audio**: Automatic sound playback on donations
- **📱 Volume Control**: Runtime volume adjustment (0-30)
- **⚡ Platform Support**: ESP32, ESP8266, Arduino compatible

## Hardware Requirements

**DFPlayer Mini MP3 Player:**
- VCC → 5V (stable power required)
- GND → Ground
- RX → ESP TX pin (GPIO4 ESP32, D1 ESP8266)  
- TX → ESP RX pin (GPIO5 ESP32, D2 ESP8266)
- Speaker → Audio output pins

**SD Card:**
- FAT32 formatted
- MP3 files: 001.mp3, 002.mp3, 003.mp3, etc.
- Donation sounds: configurable count in Config.h

## Usage

### Basic Setup

```cpp
#include "SpeakerService.hpp"

SpeakerService* speakerService;

void setup() {
    speakerService = new SpeakerService();
    
    // Robust initialization with retry logic
    bool success = speakerService->setup();
    
    if (success) {
        Serial.println("Audio system ready!");
        speakerService->playStartupSound();
    } else {
        Serial.println("Audio system unavailable - continuing without sound");
        // System continues to work without audio
    }
}

void loop() {
    speakerService->loop(); // Handle DFPlayer communication
}
```

### Donation Audio Integration

```cpp
// In your donation detection code
void onDonationDetected() {
    // Automatic random donation sound
    speakerService->playDonationSound();
    
    // Or specific sound file
    speakerService->playSound("003.mp3");
    
    // Or direct track number
    speakerService->playTrack(3);
}
```

### Volume Control

```cpp
// Set volume (0-30)
speakerService->setVolume(15);

// Runtime adjustment
speakerService->volumeUp();
speakerService->volumeDown();

// Get current volume
uint8_t currentVol = speakerService->getVolume();
```

### Production Configuration

```cpp
// Config.h settings
#define DFPLAYER_VOLUME     15      // Default volume
#define DONATION_SOUND_COUNT 5      // Number of sound files
#define STARTUP_SOUND_FILE   1      // Startup sound
#define ENABLE_SERIAL_DEBUG  0      // Production: no serial dependency
```
    Serial.begin(115200);
    
    // Initialize speaker service
    if (speaker.setup()) {
        Serial.println("Speaker service ready");
        
        // Optional: Enable debug output
        speaker.enableDebug(true);
        
        // Set volume
        speaker.setVolume(15);
    }
}

void loop() {
    // Call regularly for maintenance
    speaker.loop();
    
    // Your other code here
}
```

### Playing Sounds

```cpp
// Play donation sound (random from collection)
speaker.playDonationSound();

// Play random sound (alias for donation sound)
speaker.playRandomSound();

// Play specific sound file by number
speaker.playSound("3");  // Plays 003.mp3

// Play startup sound
speaker.playStartupSound();
```

### Volume Control

```cpp
// Set specific volume (0-30)
speaker.setVolume(20);

// Increment/decrement volume
speaker.volumeUp();
speaker.volumeDown();

// Get current volume
uint8_t volume = speaker.getVolume();
```

### Playback Control

```cpp
speaker.pause();
speaker.resume();
speaker.stop();
```

## Integration with Donation Detection

Perfect integration with the donation detection system:

```cpp
// In your main loop
if (donationDetected) {
    speaker.playDonationSound();
}
```

## Configuration

The service inherits configuration from DFPlayerService. See [DFPlayerService README](../DFPlayerService/README.md) for configuration options.

## Error Handling

- Automatically falls back to mock mode if hardware is unavailable
- All operations are safe to call even without hardware
- Logs all actions to Serial for debugging

## Dependencies

- DFPlayerService: Low-level DFPlayer Mini control
- Config.h: Pin definitions and configuration

## API Reference

### Core Methods
- `setup()`: Initialize the service
- `loop()`: Call in main loop for maintenance
- `isReady()`: Check if service is ready

### Audio Playback
- `playDonationSound()`: Play random donation sound
- `playRandomSound()`: Alias for playDonationSound()
- `playSound(String)`: Play specific sound file
- `playStartupSound()`: Play startup sound

### Volume Control
- `setVolume(uint8_t)`: Set volume (0-30)
- `getVolume()`: Get current volume
- `volumeUp()`: Increase volume
- `volumeDown()`: Decrease volume

### Playback Control
- `pause()`: Pause current playback
- `resume()`: Resume paused playback
- `stop()`: Stop current playback

### Configuration
- `enableDebug(bool)`: Enable/disable debug output
