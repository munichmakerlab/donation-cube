# ChaseMode Library

## Overview
ChaseMode creates a dynamic chase light effect with a bright moving LED followed by a trailing fade pattern. This creates the classic "chase" or "comet tail" effect commonly seen in professional lighting displays.

## Purpose
- **Chase light pattern** with moving bright spot and trailing tail
- **Comet tail effect** with gradual brightness fade behind moving light
- **Continuous motion** around the LED strip
- **High-speed chase** during donation effects for excitement

## Animation Description
- **Normal state**: Steady chase light moving around strip with fade trail
- **Donation effect**: High-speed chase for 3 seconds
- **Pattern**: Bright LED with trailing fade pattern following behind
- **Color**: Pure white with gradient brightness trail

## Visual Effect Description

### Normal Chase Pattern (30 LEDs example)
```
Chase moving clockwise with trail:

Frame 1: ●◐◑○○○○○○○○○○○○○○○○○○○○○○○○○○○○
Frame 2: ○●◐◑○○○○○○○○○○○○○○○○○○○○○○○○○○○
Frame 3: ○○●◐◑○○○○○○○○○○○○○○○○○○○○○○○○○○
Frame 4: ○○○●◐◑○○○○○○○○○○○○○○○○○○○○○○○○○
Frame 5: ○○○○●◐◑○○○○○○○○○○○○○○○○○○○○○○○○

● = Lead light (full brightness)
◐ = Trail 1 (75% brightness) 
◑ = Trail 2 (50% brightness)
○ = Trail 3+ or off (25% or off)
```

### Donation High-Speed Chase
```
Rapid chase motion:
Frame 1: ●●●●○○○○○○○○○○○○○○○○○○○○○○○○○○
Frame 2: ○○●●●●○○○○○○○○○○○○○○○○○○○○○○○○
Frame 3: ○○○○●●●●○○○○○○○○○○○○○○○○○○○○○○
Frame 4: ○○○○○○●●●●○○○○○○○○○○○○○○○○○○○○

Much faster movement with multiple bright LEDs
Intense chase effect with longer bright section
```

### Trail Fade Gradient
```
Brightness levels in trail:
Position 0 (lead):    ●●● (255 - Full brightness)
Position -1:          ◐◐◐ (205 - 80% brightness)
Position -2:          ◑◑◑ (155 - 60% brightness)
Position -3:          ◔◔◔ (105 - 40% brightness)
Position -4:          ○○○ (55  - 20% brightness)
Position -5+:         ○○○ (0   - Off)
```

## Public Functions

### Constructor
```cpp
ChaseMode(LightService* lightService, SpeakerService* speakerService)
```
**Purpose**: Initialize chase light mode  
**Parameters**: Required services for LED and audio control  
**Metadata**: 
- Name: "Chase Light"
- Description: "Moving light with trailing tail effect"
- Author: "Friedjof"
- Version: "v1.0.0"

### Core Functions (Override AbstractMode)

```cpp
void setup()
```
**Purpose**: Initialize chase light effect  
**Actions**:
- Sets up LED service with full brightness
- Configures 3-second donation effect duration
- Initializes chase position and trail parameters
- Starts initial chase animation

```cpp
void loop()
```
**Purpose**: Update chase light animation  
**Behavior**:
- Checks for donation effect completion
- Updates chase position at timed intervals
- Manages trail fade effect behind moving light
- Handles speed changes during donation effects

```cpp
void donationTriggered()
```
**Purpose**: Handle donation detection  
**Actions**:
- Starts donation effect timer
- Switches to high-speed chase mode
- Plays audio feedback ("chase.mp3")
- Creates rapid chase motion for visual excitement

## Chase Effect Logic

### Trail Calculation
```cpp
// Chase with fade trail
int trailLength = 5; // Number of LEDs in trail

for (int i = 0; i < trailLength; i++) {
    int ledIndex = (chasePosition - i + NUM_LEDS) % NUM_LEDS;
    uint8_t brightness = 255 - (i * 50); // Fade calculation
    
    CRGB color = CRGB::White;
    color.fadeToBlackBy(255 - brightness);
    lightService->setLedColor(ledIndex, color);
}
```

### Movement Pattern
- **Lead light**: Full brightness at chase position
- **Trail lights**: Decreasing brightness behind lead
- **Continuous motion**: Chase moves around LED ring
- **Smooth fading**: Gradual brightness reduction in trail

## Usage Example

```cpp
// Create chase light mode
ChaseMode* chaseMode = new ChaseMode(lightService, speakerService);

// Add to controller
controller->addMode(chaseMode);

// Mode will automatically:
// - Start chase animation when activated
// - Handle donation detection
// - Accelerate chase during donations
// - Return to normal speed after effect
// - Deactivate after donation effect completes
```

## Technical Implementation

### Chase Algorithm
```cpp
void updateChase() {
    // Clear all LEDs first
    lightService->clear();
    
    // Draw trail behind chase position
    for (int i = 0; i < trailLength; i++) {
        // Calculate LED position with wrap-around
        int ledIndex = (chasePosition - i + NUM_LEDS) % NUM_LEDS;
        
        // Calculate brightness (linear fade)
        uint8_t brightness = 255 - (i * fadeStep);
        
        // Apply brightness to white LED
        CRGB color = CRGB::White;
        color.fadeToBlackBy(255 - brightness);
        lightService->setLedColor(ledIndex, color);
    }
    
    // Move chase position
    chasePosition = (chasePosition + 1) % NUM_LEDS;
}
```

### Speed Control
- **Normal speed**: Smooth, visible chase motion
- **Donation speed**: Rapid chase for excitement
- **Continuous motion**: No stops or pauses
- **Wrap-around**: Seamless transition at strip ends

## Animation Parameters

### Trail Configuration
- **Trail length**: Number of LEDs in fade trail (typically 4-6)
- **Fade step**: Brightness reduction per trail position
- **Lead brightness**: Full brightness for chase head
- **Minimum brightness**: Cutoff point for trail visibility

### Timing Control
- **Update interval**: Controls chase speed
- **Position increment**: Usually 1 LED per update
- **Direction**: Clockwise motion around strip
- **Consistency**: Stable timing for smooth motion

## Customization Options

### Trail Length
```cpp
// Short tail (fast, punchy)
int trailLength = 3;

// Medium tail (balanced)
int trailLength = 5;

// Long tail (dramatic comet)
int trailLength = 8;
```

### Fade Pattern
```cpp
// Linear fade
brightness = 255 - (i * 50);

// Exponential fade (more dramatic)
brightness = 255 * pow(0.7, i);

// Stepped fade (retro look)
brightness = 255 - (i * 85);
```

## Ideal Use Cases
- **Dynamic displays**: Eye-catching motion for visibility
- **Professional lighting**: Classic chase effect for events
- **Retro aesthetics**: Nostalgic chase light patterns
- **High-energy environments**: Exciting, kinetic visual effects
- **Attention direction**: Moving light guides eye movement

## Visual Impact
- **Kinetic energy**: Sense of motion and activity
- **Professional appearance**: Classic lighting effect
- **Smooth motion**: Pleasing, hypnotic movement
- **Trail drama**: Comet-like tail creates elegance
- **Continuous flow**: Never-ending circular motion

## Dependencies
- AbstractMode (base functionality)
- LightService (LED control)
- SpeakerService (audio feedback)
- Config.h (NUM_LEDS and timing constants)
