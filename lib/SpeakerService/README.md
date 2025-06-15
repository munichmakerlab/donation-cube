# Speaker Service

High-level audio interface for the Donation Box project using DFPlayer Mini MP3 player.

## Overview

The SpeakerService provides a simple, high-level API for audio playback in the donation box. It wraps the DFPlayerService with donation-specific functionality and provides an easy-to-use interface for the main application.

## Features

- **Simple API**: Easy-to-use methods for common audio tasks
- **Donation-specific**: Built-in donation sound handling
- **Volume Control**: Simple volume management
- **Error Resilient**: Graceful handling of hardware issues
- **Mock Support**: Works without hardware for testing

## Hardware

This service uses the DFPlayer Mini MP3 player module. See the [DFPlayerService documentation](../DFPlayerService/README.md) for detailed hardware setup instructions.

## Usage

### Basic Setup

```cpp
#include "SpeakerService.hpp"

SpeakerService speaker;

void setup() {
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
