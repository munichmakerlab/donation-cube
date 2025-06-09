# BlinkMode Library

## Overview
BlinkMode creates a random blinking pattern with white LEDs, generating unpredictable and lively lighting effects. LEDs randomly illuminate and fade, creating dynamic sparkle patterns.

## Purpose
- **Random blinking** with unpredictable LED activation
- **Sparkle effect** creating lively, energetic lighting
- **Variable intensity** during donation effects
- **Organic randomness** for natural, non-repetitive patterns

## Animation Description
- **Normal state**: Random LEDs blink on and off periodically
- **Donation effect**: Intense random blinking for 3 seconds
- **Pattern**: Unpredictable LED activation with random selection
- **Color**: Pure white with random brightness variations

## Public Functions

### Constructor
```cpp
BlinkMode(LightService* lightService, SpeakerService* speakerService)
```
**Purpose**: Initialize random blink mode  
**Parameters**: Required services for LED and audio control  
**Metadata**: 
- Name: "Random Blink"
- Description: "Random blinking pattern with white LEDs"
- Author: "Friedjof"
- Version: "v1.0.0"

### Core Functions (Override AbstractMode)

```cpp
void setup()
```
**Purpose**: Initialize blinking effect  
**Actions**:
- Sets up LED service with full brightness
- Configures 3-second donation effect duration
- Initializes random seed for unpredictable patterns
- Starts initial blinking animation

```cpp
void loop()
```
**Purpose**: Update blinking animation  
**Behavior**:
- Checks for donation effect completion
- Updates random blinking at regular intervals
- Manages LED activation probability
- Handles intensity changes during donation effects

```cpp
void donationTriggered()
```
**Purpose**: Handle donation detection  
**Actions**:
- Starts donation effect timer
- Increases blinking intensity and frequency
- Plays audio feedback ("blink.mp3")
- Creates intense sparkle effect for visual excitement

## Blinking Parameters

### Normal Blinking
- **Activation probability**: Low chance per LED per update
- **Random selection**: Each LED independently evaluated
- **Timing**: Regular update intervals for smooth randomness
- **Brightness**: Random variation within white spectrum

### Donation Blinking
- **High intensity**: Increased activation probability
- **Rapid updates**: Faster refresh rate for excitement
- **More LEDs**: Higher chance of multiple simultaneous blinks
- **Enhanced brightness**: Full brightness variations

## Usage Example

```cpp
// Create random blink mode
BlinkMode* blinkMode = new BlinkMode(lightService, speakerService);

// Add to controller
controller->addMode(blinkMode);

// Mode will automatically:
// - Start random blinking when activated
// - Handle donation detection
// - Intensify blinking during donations
// - Return to normal pattern after effect
// - Deactivate after donation effect completes
```

## Visual Effect Description

### Normal Random Blinking
```
LED Strip over time:
Frame 1: ○ ● ○ ○ ○ ○ ● ○ ○ ○
Frame 2: ○ ○ ○ ● ○ ○ ○ ○ ○ ●
Frame 3: ● ○ ○ ○ ○ ● ○ ○ ○ ○
Frame 4: ○ ○ ● ○ ○ ○ ○ ● ○ ○

● = LED ON (randomly selected)
○ = LED OFF
Pattern: Unpredictable, organic
```

### Donation Intensive Blinking
```
LED Strip during donation:
Frame 1: ● ● ○ ● ● ○ ● ○ ● ●
Frame 2: ○ ● ● ○ ● ● ● ● ○ ●
Frame 3: ● ○ ● ● ○ ● ○ ● ● ○
Frame 4: ● ● ● ○ ● ○ ● ● ● ●

Much higher density of active LEDs
Rapid, intense sparkle effect
```

## Randomness Algorithm

### LED Selection Process
```cpp
// Simplified random selection
for (int i = 0; i < NUM_LEDS; i++) {
    if (random(100) < activationChance) {
        // Random brightness variation
        uint8_t brightness = random(128, 255);
        CRGB color = CRGB::White;
        color.fadeToBlackBy(255 - brightness);
        lightService->setLedColor(i, color);
    } else {
        lightService->setLedColor(i, CRGB::Black);
    }
}
```

### Probability Control
- **Normal mode**: Low activation chance (e.g., 5-10%)
- **Donation mode**: High activation chance (e.g., 30-50%)
- **Random brightness**: Variation within white spectrum
- **Independent evaluation**: Each LED decided separately

## Technical Details

### Randomization
- **True randomness**: Uses Arduino's random() function
- **Seed initialization**: Random seed for unpredictable patterns
- **Per-LED evaluation**: Each LED independently chosen
- **Brightness variation**: Random intensity for natural effect

### Timing Control
- **Update frequency**: Regular intervals for smooth animation
- **Donation acceleration**: Faster updates during effects
- **Frame consistency**: Stable timing for predictable behavior

### Memory Efficiency
- **No pattern storage**: Pure algorithmic generation
- **Low memory usage**: No complex state tracking
- **Real-time generation**: All patterns computed on-demand

## Ideal Use Cases
- **Party environments**: Energetic, festive lighting
- **Youth events**: Dynamic, exciting visual effects
- **Attention-grabbing**: High visibility for fundraising
- **Interactive feedback**: Clear response to donations
- **Celebration mode**: Exciting reward for contributors

## Customization Options
- **Activation probability**: Adjust blinking frequency
- **Brightness range**: Control intensity variations
- **Update timing**: Modify animation speed
- **Donation intensity**: Configure excitement level

## Dependencies
- AbstractMode (base functionality)
- LightService (LED control)
- SpeakerService (audio feedback)
- Arduino random() functions (randomization)
- Config.h (NUM_LEDS and timing constants)
