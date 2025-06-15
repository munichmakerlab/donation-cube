# StaticMode

Gentle breathing animation with white LEDs and integrated audio feedback for donation detection.

## Overview

StaticMode provides a professional breathing effect that creates a calm, welcoming atmosphere. When donations are detected, it accelerates the breathing pattern and plays audio feedback for 3 seconds.

## ✨ Features

- **🫁 Breathing Animation**: Smooth, natural breathing pattern
- **🎵 Audio Integration**: Automatic donation sound playback  
- **⚡ Dynamic Speed**: Faster breathing during donation effects
- **⚪ Pure White LEDs**: Professional, clean appearance
- **🎯 3-Second Effects**: Perfect timing for donation acknowledgment

## Animation Behavior

### Normal State
- **Pattern**: Gentle breathing effect with all LEDs
- **Color**: Pure white with smooth brightness transitions
- **Speed**: Calm, relaxing pace (normal interval)
- **Range**: Dims to ~20% brightness, peaks at 100%

### Donation Effect (3 seconds)
- **Pattern**: Accelerated breathing animation
- **Audio**: Random donation sound from SD card
- **Speed**: Faster breathing for visual impact
- **Auto-end**: Returns to normal speed after effect

## Visual Pattern

```
Normal Breathing:
    ▲ Brightness
255 |     ●●●     ●●●
    |   ●     ●   ●     ●
    | ●         ● ●       ●
 50 |●           ●         ●
    └─────────────────────────► Time
    
Donation Breathing:
    ▲ Brightness  
255 |●●●●●●●●●●●●●●●●●●●●●●●
    |Faster transitions + Audio
 50 |●●●●●●●●●●●●●●●●●●●●●●●
    └─────────────────────────► Time
```
    Fast, excited breathing
```

## Public Functions

### Constructor
```cpp
StaticMode(LightService* lightService, SpeakerService* speakerService)
```
**Purpose**: Initialize static breathing mode  
**Parameters**: Required services for LED and audio control  
**Metadata**: 
- Name: "Static Breathing"
- Description: "Gentle breathing effect with white LEDs"
- Author: "Friedjof"
- Version: "v1.0.0"

### Core Functions (Override AbstractMode)

```cpp
void setup()
```
**Purpose**: Initialize breathing effect  
**Actions**:
- Sets up LED service with full brightness
- Configures 3-second donation effect duration
- Initializes breathing parameters
- Starts initial breathing animation

```cpp
void loop()
```
**Purpose**: Update breathing animation  
**Behavior**:
- Checks for donation effect completion
- Updates breathing animation at appropriate intervals
- Manages brightness transitions smoothly
- Handles speed changes during donation effects

```cpp
void donationTriggered()
```
**Purpose**: Handle donation detection  
**Actions**:
- Starts donation effect timer
- Switches to faster breathing speed
- Plays audio feedback ("static.mp3")
- Provides visual confirmation of donation

## Animation Parameters

### Breathing Speed
- **Normal breathing**: Slow, calming pace (BREATH_SPEED_NORMAL)
- **Donation breathing**: Fast, excited pace (BREATH_SPEED_FAST)
- **Transition**: Immediate speed change when donation detected

### Brightness Range
- **Minimum**: MIN_BRIGHTNESS (dim but visible)
- **Maximum**: 255 (full brightness)
- **Transition**: Smooth linear brightness changes

## Usage Example

```cpp
// Create static breathing mode
StaticMode* staticMode = new StaticMode(lightService, speakerService);

// Add to controller
controller->addMode(staticMode);

// Mode will automatically:
// - Start breathing effect when activated
// - Handle donation detection
// - Switch to faster breathing during donations
// - Return to normal speed after effect
// - Deactivate after donation effect completes
```

## Technical Details

### Timing
- **Update interval**: Controlled by breathing speed settings
- **Smooth transitions**: Linear brightness interpolation
- **Donation duration**: 3 seconds of accelerated breathing

### LED Management
- **All LEDs synchronized**: Uniform breathing across entire strip
- **Color consistency**: Pure white (CRGB::White) only
- **Brightness control**: Uses fadeToBlackBy() for smooth dimming

### Integration
- **Automatic activation**: Ready when controller activates mode
- **Sensor integration**: Responds to donation detection immediately
- **Audio feedback**: Plays mode-specific sound during donations
- **Clean transitions**: Smooth handoff to next mode

## Ideal Use Cases
- **Default mode**: Gentle, welcoming appearance
- **Calm environments**: Libraries, museums, quiet spaces
- **Professional settings**: Corporate donation drives
- **Accessibility**: Clear visual indication without distraction

## Dependencies
- AbstractMode (base functionality)
- LightService (LED control)
- SpeakerService (audio feedback)
- Config.h (timing and brightness constants)
