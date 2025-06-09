# WaveMode Library

## Overview
WaveMode creates a dynamic wave effect that moves through the LED strip, simulating flowing motion with white LEDs. The wave travels continuously around the strip with trailing brightness gradients.

## Purpose
- **Wave animation** with moving light patterns
- **Dynamic motion** creating flowing visual effects
- **Gradient trails** with brightness variations for depth
- **Accelerated waves** during donation effects for excitement

## Animation Description
- **Normal state**: Smooth wave traveling around LED strip
- **Donation effect**: Rapid wave motion for 3 seconds
- **Pattern**: Moving bright spots with trailing gradient
- **Color**: Pure white with brightness gradients for wave effect

## Visual Effect Description

### Normal Wave Motion
```
LED Strip (top view):
    LED: 0 1 2 3 4 5 6 7 8 9 ...
Frame 1: ● ○ ○ ○ ○ ○ ○ ○ ○ ○
Frame 2: ○ ● ○ ○ ○ ○ ○ ○ ○ ○
Frame 3: ○ ○ ● ○ ○ ○ ○ ○ ○ ○
Frame 4: ○ ○ ○ ● ○ ○ ○ ○ ○ ○
...      (wave continues around ring)

● = Bright LED    ○ = Dim/Off LED
```

### Donation Wave Motion
```
LED Strip during donation:
    LED: 0 1 2 3 4 5 6 7 8 9 ...
Fast:   ●●●●●●●●●●●●●●●●●●●●●●
Motion: Wave moves much faster
        Multiple waves may be visible
```

### Wave Gradient Effect
```
Wave with trailing gradient:
    LED: 0 1 2 3 4 5 6 7 8 9 ...
Pattern: ○ ○ ● ◐ ◑ ○ ○ ○ ○ ○

● = Full brightness (wave center)
◐ = 75% brightness (wave trail)
◑ = 50% brightness (wave trail)
○ = Dim base lighting
```

## Public Functions

### Constructor
```cpp
WaveMode(LightService* lightService, SpeakerService* speakerService)
```
**Purpose**: Initialize wave motion mode  
**Parameters**: Required services for LED and audio control  
**Metadata**: 
- Name: "Wave Motion"
- Description: "Wave effect moving through LED strip"
- Author: "Friedjof"
- Version: "v1.0.0"

### Core Functions (Override AbstractMode)

```cpp
void setup()
```
**Purpose**: Initialize wave effect  
**Actions**:
- Sets up LED service with full brightness
- Configures 3-second donation effect duration
- Initializes wave parameters (position, width, speed)
- Starts initial wave animation

```cpp
void loop()
```
**Purpose**: Update wave animation  
**Behavior**:
- Checks for donation effect completion
- Updates wave position at timed intervals
- Manages wave movement and rendering
- Handles speed changes during donation effects

```cpp
void donationTriggered()
```
**Purpose**: Handle donation detection  
**Actions**:
- Starts donation effect timer
- Switches to faster wave speed
- Plays audio feedback ("wave.mp3")
- Accelerates wave motion for visual excitement

## Wave Parameters

### Movement Speed
- **Normal speed**: Smooth, visible wave progression
- **Fast speed**: Rapid wave motion during donations
- **Continuous motion**: Wave never stops, only changes speed

### Wave Characteristics
- **Wave width**: Multiple LEDs create trailing effect
- **Position tracking**: Continuous movement around LED ring
- **Gradient effect**: Brightness fades within wave for depth

## Usage Example

```cpp
// Create wave motion mode
WaveMode* waveMode = new WaveMode(lightService, speakerService);

// Add to controller
controller->addMode(waveMode);

// Mode will automatically:
// - Start wave motion when activated
// - Handle donation detection
// - Accelerate wave during donations
// - Return to normal speed after effect
// - Deactivate after donation effect completes
```

## Technical Details

### Wave Movement
- **Position increment**: Wave moves one LED position per update
- **Ring wrapping**: Wave seamlessly wraps around LED strip
- **Continuous motion**: No gaps or stops in wave movement
- **Smooth timing**: Consistent update intervals

### Rendering Process
1. **Clear base**: Set all LEDs to dim white base lighting
2. **Calculate positions**: Determine wave LED positions
3. **Apply gradient**: Set brightness levels within wave
4. **Update display**: Automatically refresh LED strip

### Speed Control
- **Normal timing**: Comfortable viewing speed
- **Donation timing**: Significantly faster for excitement
- **Immediate switching**: Speed changes instantly on donation

## Animation Algorithm
```cpp
// Simplified wave rendering
for (int i = 0; i < waveWidth; i++) {
    int ledIndex = (wavePosition + i) % NUM_LEDS;
    uint8_t brightness = 255 - (i * brightnessDecrement);
    CRGB color = CRGB::White;
    color.fadeToBlackBy(255 - brightness);
    lightService->setLedColor(ledIndex, color);
}
```

## Ideal Use Cases
- **Dynamic displays**: Eye-catching motion for visibility
- **Modern environments**: Tech-forward, contemporary spaces
- **High-traffic areas**: Visible movement draws attention
- **Interactive displays**: Clear response to user interaction

## Dependencies
- AbstractMode (base functionality)
- LightService (LED control)
- SpeakerService (audio feedback)
- Config.h (NUM_LEDS and timing constants)
