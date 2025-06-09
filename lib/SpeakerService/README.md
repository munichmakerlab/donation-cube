# SpeakerService Library

## Overview
SpeakerService provides audio playback functionality for the donation box system. Currently implemented as a mock service with Serial output, designed for future integration with audio hardware like DFPlayer Mini.

## Purpose
- **Audio feedback** for donation events
- **Sound file management** with named audio files
- **Random sound selection** for variety
- **Mock implementation** ready for hardware integration
- **Future-proof interface** for easy audio hardware addition

## Public Functions

### Constructor
```cpp
SpeakerService()
```
**Purpose**: Initialize speaker service  
**Usage**: Create instance before calling setup()

### Setup and Initialization
```cpp
void setup()
```
**Purpose**: Initialize audio hardware (currently mock)  
**Must call**: Before using audio functions  
**Future**: Will configure DFPlayer Mini or similar audio module

### Sound Playback
```cpp
void playSound(const String& soundFile)
```
**Purpose**: Play specific audio file  
**Parameters**: `soundFile` - Name of audio file to play (e.g., "donation.mp3")  
**Current**: Prints sound name to Serial monitor  
**Future**: Will play actual audio file

```cpp
void playRandomSound()
```
**Purpose**: Play random sound from predefined collection  
**Current**: Prints random sound selection to Serial  
**Future**: Will randomly select and play audio file

## Usage Examples

### Basic Setup
```cpp
SpeakerService* speakerService = new SpeakerService();
speakerService->setup();
```

### Play Specific Sound
```cpp
// Play donation sound
speakerService->playSound("donation.mp3");

// Play mode-specific sounds
speakerService->playSound("wave.mp3");
speakerService->playSound("blink.mp3");
speakerService->playSound("chase.mp3");
```

### Random Sound Selection
```cpp
// Play random sound for variety
speakerService->playRandomSound();
```

### Integration with Modes
```cpp
class MyMode : public AbstractMode {
    void donationTriggered() override {
        startDonationEffect();
        
        // Play mode-specific sound
        speakerService->playSound("mymode.mp3");
        
        // Or play random sound
        // speakerService->playRandomSound();
    }
};
```

### Complete Integration Example
```cpp
void setup() {
    speakerService->setup();
    Serial.println("Speaker service initialized");
}

void onDonationDetected() {
    Serial.println("*** DONATION DETECTED! ***");
    
    // Trigger visual effect
    currentMode->donationTriggered();
    
    // Play audio feedback
    speakerService->playSound("donation_success.mp3");
}

void onModeSwitch(const String& modeName) {
    Serial.println("Switching mode to: " + modeName);
    
    // Play mode-specific intro sound
    if (modeName == "Wave Motion") {
        speakerService->playSound("wave_intro.mp3");
    } else if (modeName == "Static Breathing") {
        speakerService->playSound("breath_intro.mp3");
    } else {
        // Play generic mode switch sound
        speakerService->playSound("mode_switch.mp3");
    }
}
```

## Current Implementation (Mock)
The service currently provides Serial output for debugging:

```
[SPEAKER] Playing sound: donation.mp3
[SPEAKER] Playing random sound: cheer.mp3
[SPEAKER] Audio system ready
```

## Future Hardware Integration

### Planned Audio Hardware
- **DFPlayer Mini**: MP3 module with SD card support
- **I2S Audio**: High-quality digital audio output
- **Simple Buzzer**: Basic beep sounds for minimal setups

### Expected Audio Files
```
sounds/
├── donation.mp3          # Main donation sound
├── mode_switch.mp3       # Mode change sound
├── wave.mp3             # Wave mode sound
├── blink.mp3            # Blink mode sound
├── chase.mp3            # Chase mode sound
├── static.mp3           # Static mode sound
├── center.mp3           # Center mode sound
├── half.mp3             # Half mode sound
├── cheer.mp3            # Random sound 1
├── success.mp3          # Random sound 2
└── thanks.mp3           # Random sound 3
```

### Hardware Wiring (Future)
```
DFPlayer Mini →  ESP32-C3    | Description
--------------|--------------|-------------
VCC           →  5V          | Power supply
GND           →  GND         | Ground
TX            →  GPIO_RX     | Serial communication
RX            →  GPIO_TX     | Serial communication
SPK+          →  Speaker+    | Audio output
SPK-          →  Speaker-    | Audio output
```

## Integration Benefits
- **User Feedback**: Audio confirmation of donations
- **Mode Identification**: Unique sounds for each mode
- **Enhanced Experience**: Professional audio-visual feedback
- **Accessibility**: Audio cues for visually impaired users

## Development Notes
- **Mock Ready**: Current implementation ready for testing
- **Hardware Agnostic**: Interface works with various audio modules
- **Easy Upgrade**: Replace mock with hardware implementation
- **Debug Friendly**: Serial output shows all audio events

## Dependencies
- Config.h (future pin definitions)
- Arduino.h (basic functions)
- Future: SoftwareSerial (for DFPlayer Mini communication)
