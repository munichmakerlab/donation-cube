# HalfMode Library

## Overview
HalfMode creates an alternating illumination pattern between the first and second half of the LED strip. This creates a distinctive back-and-forth visual effect perfect for drawing attention.

## Purpose
- **Alternating halves** switching between first and second half
- **Clear visual rhythm** with predictable switching pattern
- **High visibility** through contrasting illumination zones
- **Rapid switching** during donation effects for excitement

## Animation Description
- **Normal state**: Slow alternation between first and second half
- **Donation effect**: Fast switching for 3 seconds
- **Pattern**: Clean division of LED strip into two zones
- **Color**: Pure white with full brightness contrast

## Visual Effect Description

### Normal Half Switching (30 LEDs example)
```
State A - First Half ON:
LEDs 0-14:  ●●●●●●●●●●●●●●●
LEDs 15-29: ○○○○○○○○○○○○○○○

State B - Second Half ON:
LEDs 0-14:  ○○○○○○○○○○○○○○○
LEDs 15-29: ●●●●●●●●●●●●●●●

● = LEDs ON (full brightness)
○ = LEDs OFF
```

### Donation Fast Switching
```
Rapid alternation:
Time 0.0s: ●●●●●●●●●●●●●●●○○○○○○○○○○○○○○○
Time 0.1s: ○○○○○○○○○○○○○○○●●●●●●●●●●●●●●●
Time 0.2s: ●●●●●●●●●●●●●●●○○○○○○○○○○○○○○○
Time 0.3s: ○○○○○○○○○○○○○○○●●●●●●●●●●●●●●●

Much faster switching creates strobe-like effect
```

## Public Functions

### Constructor
```cpp
HalfMode(LightService* lightService, SpeakerService* speakerService)
```
**Purpose**: Initialize half switching mode  
**Parameters**: Required services for LED and audio control  
**Metadata**: 
- Name: "Half Switch"
- Description: "Alternating first and second half illumination"
- Author: "Friedjof"
- Version: "v1.0.0"

### Core Functions (Override AbstractMode)

```cpp
void setup()
```
**Purpose**: Initialize half switching effect  
**Actions**:
- Sets up LED service with full brightness
- Configures 3-second donation effect duration
- Calculates LED strip midpoint for half division
- Starts initial alternating pattern

```cpp
void loop()
```
**Purpose**: Update half switching animation  
**Behavior**:
- Checks for donation effect completion
- Updates half switching at timed intervals
- Toggles between first and second half illumination
- Handles speed changes during donation effects

```cpp
void donationTriggered()
```
**Purpose**: Handle donation detection  
**Actions**:
- Starts donation effect timer
- Switches to rapid alternation speed
- Plays audio feedback ("half.mp3")
- Creates fast switching for visual excitement

## Half Division Logic

### LED Strip Division
```cpp
// Strip division calculation
int halfPoint = NUM_LEDS / 2;

// First half: LEDs 0 to (halfPoint - 1)
// Second half: LEDs halfPoint to (NUM_LEDS - 1)
```

### Switching States
- **State A**: First half ON, second half OFF
- **State B**: First half OFF, second half ON
- **Alternation**: Regular switching between states
- **Clean transitions**: Immediate state changes

## Usage Example

```cpp
// Create half switching mode
HalfMode* halfMode = new HalfMode(lightService, speakerService);

// Add to controller
controller->addMode(halfMode);

// Mode will automatically:
// - Start half alternation when activated
// - Handle donation detection
// - Accelerate switching during donations
// - Return to normal speed after effect
// - Deactivate after donation effect completes
```

## Animation Timing

### Normal Speed
- **Switch interval**: Comfortable viewing pace
- **Clear transitions**: Easy to follow alternation
- **Rhythmic pattern**: Predictable timing

### Donation Speed
- **Rapid switching**: High-frequency alternation
- **Strobe effect**: Fast enough to create excitement
- **Attention-grabbing**: Impossible to ignore

## Technical Implementation

### Half Calculation
```cpp
// Calculate half point (handles odd/even LED counts)
int halfPoint = NUM_LEDS / 2;

// State A: Light first half
for (int i = 0; i < halfPoint; i++) {
    lightService->setLedColor(i, CRGB::White);
}
for (int i = halfPoint; i < NUM_LEDS; i++) {
    lightService->setLedColor(i, CRGB::Black);
}

// State B: Light second half
for (int i = 0; i < halfPoint; i++) {
    lightService->setLedColor(i, CRGB::Black);
}
for (int i = halfPoint; i < NUM_LEDS; i++) {
    lightService->setLedColor(i, CRGB::White);
}
```

### State Management
- **Boolean toggle**: Simple true/false state tracking
- **Timer-based**: Switching based on time intervals
- **Immediate updates**: No gradual transitions
- **Clean divisions**: Sharp boundary between halves

## Flexibility Features

### Odd LED Counts
- **Automatic handling**: Midpoint calculation works with any count
- **Extra LED**: Goes to second half for odd numbers
- **Examples**: 
  - 30 LEDs: 15+15
  - 31 LEDs: 15+16

### Customizable Timing
- **Normal interval**: Adjustable switching speed
- **Donation interval**: Configurable excitement level
- **Easy modification**: Simple timing constants

## Ideal Use Cases
- **High visibility**: Strong contrast draws attention
- **Directional indication**: Can suggest back-and-forth motion
- **Alert mode**: Clear notification of system status
- **Simple pattern**: Easy to understand and follow
- **Professional display**: Clean, geometric appearance

## Visual Impact
- **High contrast**: Maximum visual difference between states
- **Clear rhythm**: Predictable, easy-to-follow pattern
- **Strong presence**: Difficult to ignore when active
- **Professional look**: Clean, organized appearance

## Dependencies
- AbstractMode (base functionality)
- LightService (LED control)
- SpeakerService (audio feedback)
- Config.h (NUM_LEDS and timing constants)
