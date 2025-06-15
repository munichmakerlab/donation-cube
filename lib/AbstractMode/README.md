# AbstractMode

Base class foundation for all LED animation modes with integrated audio feedback and effect management.

## Overview

AbstractMode provides the standardized foundation for all LED animation modes in the donation box system. It handles donation effect lifecycle, audio integration, metadata management, and ensures consistent behavior across all modes.

## ✨ Core Features

- **🏗️ Base Class Pattern**: Standard foundation for all modes
- **⏱️ Effect Management**: Automatic 3-second donation effect timing
- **🎵 Audio Integration**: Built-in SpeakerService coordination
- **📋 Mode Metadata**: Name, description, author, version tracking
- **🔄 Lifecycle Control**: Setup, loop, activation, deactivation
- **🎯 Donation Handling**: Standardized donation trigger interface

## Architecture Pattern

```cpp
class YourMode : public AbstractMode {
public:
    YourMode(LightService* light, SpeakerService* speaker) 
        : AbstractMode(light, speaker, 3000, // 3s effect duration
                      "Your Mode", "Description", "Author", "v1.0.0") {}
                      
    void setup() override;           // Initialize animation
    void loop() override;            // Main animation loop  
    void donationTriggered() override; // Handle donation
};
```

## Essential Methods for Implementation

### Required Override Methods
```cpp
virtual void setup() = 0;
virtual void loop() = 0; 
virtual void donationTriggered() = 0;
```

### Donation Effect Pattern
```cpp
void YourMode::donationTriggered() {
    startDonationEffect();              // REQUIRED: Start timing
    speakerService->playDonationSound(); // REQUIRED: Audio feedback
    // Your custom donation animation changes
}

void YourMode::loop() {
    // CRITICAL: Auto-end donation effect
    if (isDonationEffectActive() && 
        millis() - getDonationStartTime() >= getEffectDuration()) {
        endDonationEffect(); // Triggers mode switch
        return;
    }
    // Your animation logic here
}
```
```cpp
void startDonationEffect()
```
**Purpose**: Start donation effect timer (must be called in `donationTriggered()`)  
**Usage**: Call when donation is detected to begin special effect

```cpp
void endDonationEffect()
```
**Purpose**: End donation effect and deactivate mode  
**Usage**: Automatically called when effect duration expires

```cpp
bool isDonationEffectActive() const
```
**Purpose**: Check if donation effect is currently running  
**Returns**: `true` if donation effect is active

```cpp
unsigned long getEffectDuration() const
```
**Purpose**: Get donation effect duration in milliseconds  
**Returns**: Duration (default 3000ms, can be overridden)

### Mode Metadata
```cpp
const String& getName() const
const String& getDescription() const  
const String& getAuthor() const
const String& getVersion() const
```
**Purpose**: Access mode metadata for debugging and display  
**Returns**: Respective metadata strings

```cpp
void printModeInfo() const
```
**Purpose**: Print formatted mode information to Serial monitor  
**Usage**: Called automatically during mode switches

### Mode Lifecycle
```cpp
bool isActive()
void activate()
void deactivate()
```
**Purpose**: Manage mode active state  
**Usage**: Controlled by Controller class

### Virtual Functions (Must Override)
```cpp
virtual void setup() = 0
```
**Purpose**: One-time mode initialization  
**Must implement**: LED setup, variable initialization

```cpp
virtual void loop() = 0  
```
**Purpose**: Continuous animation logic  
**Must implement**: Animation updates, donation effect timing checks

```cpp
virtual void donationTriggered() = 0
```
**Purpose**: Handle donation detection  
**Must implement**: Call `startDonationEffect()` and trigger special effect

## Usage Example
```cpp
class MyMode : public AbstractMode {
public:
    MyMode(LightService* ls, SpeakerService* ss) 
        : AbstractMode(ls, ss, "My Mode", "Cool effect", "Author", "v1.0") {}
    
    void setup() override {
        lightService->setup();
        effectDuration = 4000; // 4 second donation effect
    }
    
    void loop() override {
        // Check donation effect end
        if (effectActive && millis() - effectStartTime >= effectDuration) {
            endDonationEffect();
        }
        // Your animation logic here
    }
    
    void donationTriggered() override {
        startDonationEffect(); // Required!
        speakerService->playSound("sound.mp3");
    }
};
```

## Protected Members
- `effectStartTime` - When donation effect started (milliseconds)
- `effectActive` - Boolean flag for donation effect state
- `effectDuration` - Duration of donation effect (customizable)
- `lightService` - Access to LED control
- `speakerService` - Access to audio playback

## Dependencies
- LightService (LED control)
- SpeakerService (audio playback)
- Config.h (hardware configuration)
