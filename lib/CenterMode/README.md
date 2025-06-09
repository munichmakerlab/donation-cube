# CenterMode Library

## Overview
CenterMode creates an expanding light effect that starts from the center of the LED strip and spreads outward symmetrically. This creates a visually striking radial expansion pattern perfect for dramatic visual effects.

## Purpose
- **Center expansion** starting from middle of LED strip
- **Symmetric spreading** outward in both directions
- **Radial growth pattern** creating expanding circles of light
- **Rapid expansion** during donation effects for dramatic impact

## Animation Description
- **Normal state**: Slow expansion from center outward, then reset
- **Donation effect**: Rapid expansion cycles for 3 seconds
- **Pattern**: Light starts at center and grows symmetrically
- **Color**: Pure white with expanding illumination zones

## Visual Effect Description

### Normal Center Expansion (30 LEDs example)
```
Center at LED 15:

Step 1: ○○○○○○○○○○○○○○○●○○○○○○○○○○○○○○
Step 2: ○○○○○○○○○○○○○○●●●○○○○○○○○○○○○○
Step 3: ○○○○○○○○○○○○○●●●●●○○○○○○○○○○○○
Step 4: ○○○○○○○○○○○○●●●●●●●○○○○○○○○○○○
Step 5: ○○○○○○○○○○○●●●●●●●●●○○○○○○○○○○
...continues until full expansion, then resets

● = LEDs ON (expanding outward)
○ = LEDs OFF
```

### Donation Rapid Expansion
```
Fast expansion cycles:
Cycle 1: ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● (full)
Reset:   ○○○○○○○○○○○○○○○●○○○○○○○○○○○○○○ (center)
Cycle 2: ●●●●●●●●●●●●●●●●●●●●●●●●●●●●●● (full)
Reset:   ○○○○○○○○○○○○○○○●○○○○○○○○○○○○○○ (center)

Multiple rapid expansion cycles create pulsing effect
```

### Expansion Sequence Visualization
```
Radius 0: ○○○○○○○○○○○○○○○●○○○○○○○○○○○○○○
Radius 1: ○○○○○○○○○○○○○○●●●○○○○○○○○○○○○○
Radius 2: ○○○○○○○○○○○○○●●●●●○○○○○○○○○○○○
Radius 3: ○○○○○○○○○○○○●●●●●●●○○○○○○○○○○○
Radius 4: ○○○○○○○○○○○●●●●●●●●●○○○○○○○○○○
Radius 5: ○○○○○○○○○○●●●●●●●●●●●○○○○○○○○○
...continues until edges reached
```

## Public Functions

### Constructor
```cpp
CenterMode(LightService* lightService, SpeakerService* speakerService)
```
**Purpose**: Initialize center expansion mode  
**Parameters**: Required services for LED and audio control  
**Metadata**: 
- Name: "Center Expansion"
- Description: "Light expanding from center outwards"
- Author: "Friedjof"
- Version: "v1.0.0"

### Core Functions (Override AbstractMode)

```cpp
void setup()
```
**Purpose**: Initialize center expansion effect  
**Actions**:
- Sets up LED service with full brightness
- Configures 3-second donation effect duration
- Calculates LED strip center point
- Starts initial expansion animation

```cpp
void loop()
```
**Purpose**: Update center expansion animation  
**Behavior**:
- Checks for donation effect completion
- Updates expansion radius at timed intervals
- Manages symmetric LED illumination
- Handles speed changes during donation effects

```cpp
void donationTriggered()
```
**Purpose**: Handle donation detection  
**Actions**:
- Starts donation effect timer
- Switches to rapid expansion speed
- Plays audio feedback ("center.mp3")
- Creates fast expansion cycles for dramatic effect

## Expansion Logic

### Center Calculation
```cpp
// Center point calculation
int center = NUM_LEDS / 2;

// For 30 LEDs: center = 15
// Expansion spreads from LED 15 outward
```

### Symmetric Expansion
```cpp
// Expansion pattern for radius r
for (int i = 0; i <= expansionRadius; i++) {
    // Light LED to the right of center
    if (center + i < NUM_LEDS) {
        lightService->setLedColor(center + i, CRGB::White);
    }
    
    // Light LED to the left of center
    if (center - i >= 0) {
        lightService->setLedColor(center - i, CRGB::White);
    }
}
```

## Usage Example

```cpp
// Create center expansion mode
CenterMode* centerMode = new CenterMode(lightService, speakerService);

// Add to controller
controller->addMode(centerMode);

// Mode will automatically:
// - Start center expansion when activated
// - Handle donation detection
// - Accelerate expansion during donations
// - Return to normal speed after effect
// - Deactivate after donation effect completes
```

## Technical Implementation

### Expansion Algorithm
```cpp
void updateExpansion() {
    // Clear all LEDs
    lightService->clear();
    
    // Calculate center point
    int center = NUM_LEDS / 2;
    
    // Light LEDs from center outward
    for (int i = 0; i <= currentRadius; i++) {
        // Right side expansion
        if (center + i < NUM_LEDS) {
            lightService->setLedColor(center + i, CRGB::White);
        }
        
        // Left side expansion
        if (center - i >= 0) {
            lightService->setLedColor(center - i, CRGB::White);
        }
    }
    
    // Increment radius for next frame
    currentRadius++;
    
    // Reset when full expansion reached
    if (currentRadius > maxRadius) {
        currentRadius = 0;
    }
}
```

### Edge Handling
- **Boundary checking**: Prevents array out-of-bounds
- **Maximum radius**: Calculated based on strip length
- **Reset logic**: Smooth transition back to center
- **Symmetric growth**: Equal expansion in both directions

## Animation Timing

### Normal Speed
- **Expansion rate**: Comfortable viewing pace for growth
- **Reset pause**: Brief moment at center before restarting
- **Smooth progression**: Each step clearly visible

### Donation Speed
- **Rapid cycles**: Fast expansion and reset
- **Continuous motion**: Minimal pause between cycles
- **Dramatic effect**: High-energy pulsing pattern

## Flexibility Features

### Odd/Even LED Handling
- **Automatic center**: Works with any LED count
- **Single center**: For odd counts (LED at exact center)
- **Dual center**: For even counts (two center LEDs)

### Customizable Parameters
- **Expansion speed**: Adjustable timing intervals
- **Reset behavior**: Configurable pause at center
- **Maximum radius**: Automatically calculated from strip length

## Ideal Use Cases
- **Dramatic reveals**: Perfect for announcement moments
- **Celebration effects**: Exciting visual for achievements
- **Attention focusing**: Draws eyes to center point
- **Professional presentations**: Clean, organized expansion
- **Interactive feedback**: Clear response radiating from center

## Visual Impact
- **Radial symmetry**: Pleasing geometric pattern
- **Growth metaphor**: Represents expansion, growth, success
- **Center focus**: Naturally draws attention inward
- **Dramatic timing**: Build-up and release creates excitement

## Dependencies
- AbstractMode (base functionality)
- LightService (LED control)
- SpeakerService (audio feedback)
- Config.h (NUM_LEDS and timing constants)
