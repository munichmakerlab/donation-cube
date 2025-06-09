# 🎁 Modular Donation Box - LED Controller

A professional modular LED animation system for donation boxes featuring multiple lighting modes, automatic mode switching, and spectacular donation effects.

## 📋 Overview

This project implements a modular controller system for donation boxes with:
- **6 different LED animation modes** with unique effects
- **Automatic mode switching** every 2-5 seconds after donation effects
- **Professional donation detection** with edge detection and debouncing
- **Modular architecture** with services, controllers, and mode system
- **Mode metadata** for debugging and extensibility
- **White LED focus** with brightness and pattern variations

## 🎨 Animation Modes

| Mode | Name | Author | Description | Donation Effect |
|------|------|--------|-------------|-----------------|
| **1** | Static Breathing | Friedjof v1.0.0 | Gentle breathing effect with white LEDs | Faster breathing |
| **2** | Wave Motion | Friedjof v1.0.0 | Wave effect moving through LED strip | Rapid wave motion |
| **3** | Random Blink | Friedjof v1.0.0 | Random blinking pattern with white LEDs | Intense blinking |
| **4** | Half Switch | Friedjof v1.0.0 | Alternating first and second half illumination | Fast switching |
| **5** | Center Expansion | Friedjof v1.0.0 | Light expanding from center outwards | Rapid expansion |
| **6** | Chase Light | Friedjof v1.0.0 | Moving light with trailing tail effect | High-speed chase |

## 🏗️ Architecture

### Modular Design
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Controller    │───▶│   Services      │◀───│   LED Modes     │
│                 │    │                 │    │                 │
│ • Mode Manager  │    │ • LightService  │    │ • StaticMode    │
│ • Sensor Logic  │    │ • SensorService │    │ • WaveMode      │
│ • Auto Switch   │    │ • SpeakerService│    │ • BlinkMode     │
└─────────────────┘    └─────────────────┘    │ • HalfMode      │
                                              │ • CenterMode    │
                                              │ • ChaseMode     │
                                              └─────────────────┘
```

### Service Layer
- **LightService**: FastLED wrapper with CRGB color management
- **SensorService**: TCRT5000 with edge detection and debouncing  
- **SpeakerService**: Mock implementation for future sound integration

### Mode System
- **AbstractMode**: Base class with donation effect framework
- **Mode Metadata**: Name, description, author, version for each mode
- **Automatic Lifecycle**: Setup → Loop → Donation Effect → Deactivation

## 🔧 Hardware Components

| Component | Description | Purpose |
|-----------|-------------|---------|
| **ESP32-C3** | Microcontroller | Main control |
| **WS2812B LED-Strip** | Addressable white LEDs | Light effects |
| **TCRT5000 Reflection Sensor** | Infrared sensor | Donation detection |

## 🔌 Pin Configuration

| ESP32-C3 Pin | Function | Component | Description |
|--------------|----------|-----------|-------------|
| **GPIO2** | `SENSOR_PIN` | TCRT5000 | Donation sensor (INPUT_PULLUP) |
| **GPIO8** | `DATA_PIN` | WS2812B | LED strip data line |

## 🔧 Wiring

### LED-Strip (WS2812B)
```
LED-Strip →  ESP32-C3    | Description
----------|--------------|-------------
VCC       →  5V          | Power supply
GND       →  GND         | Ground
DIN       →  GPIO8       | Data line
```

### TCRT5000 Sensor
```
Sensor    →  ESP32-C3    | Description
----------|--------------|-------------
VCC       →  3.3V        | Power supply
GND       →  GND         | Ground
OUT       →  GPIO2       | Digital output
```

## 🎯 Donation Effect Sequence

| Phase | Duration | Effect | Description |
|-------|----------|--------|-------------|
| **1** | Instant | Mode-specific Effect | Current mode's donation animation |
| **2** | 2-5 seconds | Enhanced Animation | Faster/brighter version of mode |
| **3** | Instant | Mode Switch | Automatic switch to next mode |
| **4** | Continuous | New Mode | Next mode starts its normal animation |

**Features**:
- Each mode has unique donation effects
- Automatic mode progression for variety
- Professional edge detection prevents false triggers
- Mode information logged via Serial for debugging

## 🚀 Installation & Setup

### 1. Development Environment
```bash
# Install PlatformIO
pip install platformio

# Clone project
git clone <your-repo-url>
cd SpendenBox

# Install dependencies
pio lib install
```

### 2. Compile and Upload
```bash
# Compile code
pio run

# Upload to ESP32-C3
pio run --target upload

# Open serial monitor for debugging
pio device monitor --baud 115200
```

### 3. Hardware Tests
1. **LED Test**: After startup, LEDs should show first mode animation
2. **Sensor Test**: Wave hand in front of TCRT5000
3. **Mode Switch Test**: Trigger donation to see mode changes

## 📚 Used Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| [FastLED](https://github.com/FastLED/FastLED) | ≥3.6.0 | WS2812B LED strip control |

## 🛠️ Configuration

### LED Settings (`include/Config.h`)
```cpp
#define NUM_LEDS 30          // Number of LEDs in strip
#define DATA_PIN 8           // GPIO pin for LED data
#define SENSOR_PIN 2         // GPIO pin for donation sensor
```

## 🎨 Creating Your Own LED Mode

### Step-by-Step Guide

Creating a custom LED mode requires implementing three core functions and following the AbstractMode pattern. Here's a comprehensive guide:

#### 1. Create Mode Class Files

Create two files: `lib/YourMode/YourMode.hpp` and `lib/YourMode/YourMode.cpp`

**Header File (YourMode.hpp):**
```cpp
#pragma once
#include "AbstractMode.hpp"

class YourMode : public AbstractMode {
private:
    // Your private variables
    unsigned long lastUpdate;
    int currentPosition;
    bool direction;
    
public:
    YourMode(LightService* lightService, SpeakerService* speakerService);
    void setup() override;           // Called once when mode starts
    void loop() override;            // Called continuously
    void donationTriggered() override; // Called when donation detected
    
private:
    void updateAnimation(); // Your custom animation logic
};
```

**Implementation File (YourMode.cpp):**
```cpp
#include "YourMode.hpp"
#include "Config.h"
#include <Arduino.h>

YourMode::YourMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService,
                  "Your Mode Name",           // Display name
                  "Description of effect",    // What does your mode do?
                  "Your Name",                // Author
                  "v1.0.0"),                  // Version
      lastUpdate(0),
      currentPosition(0),
      direction(true) {
}

void YourMode::setup() {
    Serial.println("[INFO] YourMode setup - Your cool effect");
    lightService->setup();
    lightService->setBrightness(255);
    
    // Set donation effect duration (2-5 seconds recommended)
    effectDuration = 3000; // 3 seconds
    
    // Initialize your variables
    lastUpdate = millis();
    currentPosition = 0;
    direction = true;
    
    // Start initial animation
    updateAnimation();
}

void YourMode::loop() {
    unsigned long currentTime = millis();
    
    // CRITICAL: Check if donation effect should end
    if (effectActive && currentTime - effectStartTime >= effectDuration) {
        endDonationEffect(); // Automatically ends effect and deactivates mode
        Serial.println("[INFO] YourMode donation effect ended - mode will deactivate");
    }
    
    // Your animation timing (e.g., update every 100ms)
    if (currentTime - lastUpdate >= 100) {
        lastUpdate = currentTime;
        updateAnimation();
    }
}

void YourMode::donationTriggered() {
    Serial.println("[INFO] YourMode donation triggered - Special effect!");
    
    // CRITICAL: Start donation effect (must call this!)
    startDonationEffect();
    
    // Optional: Play sound
    speakerService->playSound("your_sound.mp3");
    
    // Optional: Activate special donation animation
    // (you can check effectActive in updateAnimation())
}

void YourMode::updateAnimation() {
    // Example: Bouncing light effect
    
    // Clear all LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
        lightService->setLedColor(i, CRGB::Black);
    }
    
    // Set current LED
    CRGB color = CRGB::White;
    if (effectActive) {
        // During donation: Full brightness, faster movement
        color.fadeToBlackBy(0); // Full brightness
    } else {
        // Normal mode: Slightly dimmed
        color.fadeToBlackBy(100);
    }
    
    lightService->setLedColor(currentPosition, color);
    
    // Move position (bouncing effect)
    if (direction) {
        currentPosition++;
        if (currentPosition >= NUM_LEDS - 1) {
            direction = false;
        }
    } else {
        currentPosition--;
        if (currentPosition <= 0) {
            direction = true;
        }
    }
}
```

#### 2. Essential Functions Explained

**setup()** - One-time initialization:
- **Must call**: `lightService->setup()` and `lightService->setBrightness()`
- **Must set**: `effectDuration` (2000-5000ms recommended)
- Initialize your variables
- Start initial animation

**loop()** - Continuous execution:
- **Critical check**: `if (effectActive && currentTime - effectStartTime >= effectDuration)`
- **Must call**: `endDonationEffect()` when effect duration expires
- Implement timing-based animation using `millis()`
- Handle your mode's normal animation logic

**donationTriggered()** - Donation event handler:
- **Must call**: `startDonationEffect()` (required for proper lifecycle)
- **Optional**: `speakerService->playSound()` for audio feedback
- Modify animation behavior using `effectActive` variable

#### 3. LED Control Examples

**Single LED Control:**
```cpp
CRGB color = CRGB::White;
color.fadeToBlackBy(128); // 50% brightness (0=full, 255=off)
lightService->setLedColor(ledIndex, color);
```

**Multiple LEDs:**
```cpp
for (int i = 0; i < NUM_LEDS; i++) {
    CRGB color = CRGB::White;
    color.fadeToBlackBy(200); // Very dim
    lightService->setLedColor(i, color);
}
```

**Brightness Variations:**
```cpp
CRGB bright = CRGB::White;    // Full brightness
CRGB dim = CRGB::White;
dim.fadeToBlackBy(150);       // Dimmed
CRGB veryDim = CRGB::White;
veryDim.fadeToBlackBy(220);   // Very dim
```

#### 4. Register Your Mode

Add to `src/main.cpp`:
```cpp
#include "YourMode.hpp"

void setup() {
    // ...existing code...
    
    // Create and register your mode
    YourMode* yourMode = new YourMode(lightService, speakerService);
    controller->addMode(yourMode);
    
    // ...existing code...
}
```

#### 5. Timing and Animation Patterns

**Basic Timing:**
```cpp
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 100; // 100ms

if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();
    // Update animation
}
```

**Variable Speed (Normal vs Donation):**
```cpp
unsigned long normalSpeed = 200;  // Normal: 200ms
unsigned long fastSpeed = 50;     // Donation: 50ms

unsigned long currentSpeed = effectActive ? fastSpeed : normalSpeed;
```

**Circular Movement:**
```cpp
currentPosition = (currentPosition + 1) % NUM_LEDS; // Loops 0->29->0
```

**Wave Patterns:**
```cpp
for (int i = 0; i < waveWidth; i++) {
    int ledIndex = (basePosition + i) % NUM_LEDS;
    uint8_t brightness = 255 - (i * 40); // Gradient fade
    CRGB color = CRGB::White;
    color.fadeToBlackBy(255 - brightness);
    lightService->setLedColor(ledIndex, color);
}
```

#### 6. Common Animation Techniques

**Breathing Effect (like StaticMode):**
```cpp
float breath = (sin(millis() / 1000.0) + 1.0) / 2.0; // 0.0 to 1.0
uint8_t brightness = (uint8_t)(255 * breath);
CRGB color = CRGB::White;
color.fadeToBlackBy(255 - brightness);
```

**Random Blinking (like BlinkMode):**
```cpp
if (random(100) < 5) { // 5% chance per update
    int randomLed = random(NUM_LEDS);
    lightService->setLedColor(randomLed, CRGB::White);
}
```

**Center Expansion (like CenterMode):**
```cpp
int center = NUM_LEDS / 2;
for (int i = 0; i <= expansionSize; i++) {
    if (center + i < NUM_LEDS) lightService->setLedColor(center + i, CRGB::White);
    if (center - i >= 0) lightService->setLedColor(center - i, CRGB::White);
}
```

#### 7. Development Tips

**Debugging:**
- Use `Serial.println()` for debug output
- Test without donation effects first
- Start with simple patterns
- Use small update intervals (50-200ms)

**Best Practices:**
- Always check `ledIndex % NUM_LEDS` for ring wrapping
- Use `effectActive` to differentiate normal vs donation behavior
- Implement smooth transitions between states
- Keep donation effects visually distinct (faster/brighter)

**Testing Checklist:**
- ✅ Mode starts correctly with `setup()`
- ✅ Animation runs smoothly in `loop()`
- ✅ Donation trigger works with `donationTriggered()`
- ✅ Effect automatically ends after `effectDuration`
- ✅ Mode switches to next mode after donation effect

### Inspiration from Existing Modes

- **StaticMode**: Smooth breathing using `sin()` functions
- **WaveMode**: Moving patterns with modulo arithmetic
- **BlinkMode**: Random selection with `random()` function
- **CenterMode**: Symmetric expansion from center point
- **ChaseMode**: Trailing effects with fade-out patterns

## 🔍 Troubleshooting

### LED Issues
```
ERROR: LEDs not responding
Check:
1. Power supply (5V for LED strip)
2. Data line connection (GPIO8)
3. Ground connection
4. LED strip compatibility (WS2812B)
```

### Sensor Issues
```
ERROR: Sensor not detecting or constantly triggering
Solutions:
1. Adjust sensor sensitivity potentiometer
2. Check sensor positioning
3. Verify 3.3V power supply
4. Test with different objects
```

### Compilation Issues
```
ERROR: Library not found
Solutions:
1. Install FastLED: pio lib install FastLED
2. Check platformio.ini configuration
3. Verify include paths
```

## 📖 Serial Monitor Output

```
[INFO] Starting modular donation box system...
[INFO] Controller setup started
========================================
           MODE INFORMATION
========================================
Name:        Static Breathing
Description: Gentle breathing effect with white LEDs
Author:      Friedjof
Version:     v1.0.0
========================================
[INFO] Activated initial mode with 6 total modes
[INFO] Setup complete. Donation box ready!

*** DONATION DETECTED! ***
[StaticMode] Donation detected - starting breathing effect
========================================
           MODE INFORMATION
========================================
Name:        Wave Motion
Description: Wave effect moving through LED strip
Author:      Friedjof
Version:     v1.0.0
========================================
[INFO] Switched to mode 1
```

## 🤝 Contributing

Improvements and extensions are welcome!

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-mode`)
3. Implement your new mode following the AbstractMode pattern
4. Add mode metadata (name, description, author, version)
5. Test your mode thoroughly
6. Commit changes (`git commit -m 'Add new lighting mode'`)
7. Push branch (`git push origin feature/new-mode`)
8. Create a pull request

### Mode Development Guidelines
- Use only white LEDs with brightness/pattern variations
- Implement unique donation effects (2-5 seconds)
- Follow naming conventions and code style
- Add comprehensive comments and documentation
- Test with real hardware before submitting

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 👥 Authors

- **Friedjof** - *Initial modular architecture* - v1.0.0

## 🎯 Roadmap

- [ ] Sound integration (DFPlayer Mini support)
- [ ] Web interface for mode configuration
- [ ] Custom timing parameters per mode
- [ ] WiFi configuration portal
- [ ] MQTT integration for remote monitoring
- [ ] Statistics and analytics
- [ ] RGB color mode option
- [ ] Temperature-based effects
- [ ] Sound-reactive modes

## 📚 Library Documentation

### Service Libraries
| Library | Purpose | Documentation |
|---------|---------|---------------|
| **AbstractMode** | Base class for all LED modes with donation effect management | [📖 AbstractMode README](lib/AbstractMode/README.md) |
| **Controller** | Mode management with automatic switching and sensor integration | [📖 Controller README](lib/Controller/README.md) |
| **LightService** | FastLED wrapper for WS2812B LED strip control | [📖 LightService README](lib/LightService/README.md) |
| **SensorService** | TCRT5000 donation detection with edge detection | [📖 SensorService README](lib/SensorService/README.md) |
| **SpeakerService** | Audio service (mock implementation for future integration) | [📖 SpeakerService README](lib/SpeakerService/README.md) |

### LED Animation Modes
| Mode | Effect Description | Documentation |
|------|-------------------|---------------|
| **StaticMode** | Gentle breathing effect with white LEDs | [📖 StaticMode README](lib/StaticMode/README.md) |
| **WaveMode** | Wave motion effect moving through LED strip | [📖 WaveMode README](lib/WaveMode/README.md) |
| **BlinkMode** | Random blinking pattern with white LEDs | [📖 BlinkMode README](lib/BlinkMode/README.md) |
| **HalfMode** | Alternating first and second half illumination | [📖 HalfMode README](lib/HalfMode/README.md) |
| **CenterMode** | Light expanding from center outwards | [📖 CenterMode README](lib/CenterMode/README.md) |
| **ChaseMode** | Moving light with trailing tail effect | [📖 ChaseMode README](lib/ChaseMode/README.md) |

### Quick Library Reference

**For developers extending the system:**
- Start with [AbstractMode README](lib/AbstractMode/README.md) to understand the base class
- Use [LightService README](lib/LightService/README.md) for LED control reference
- Check [Controller README](lib/Controller/README.md) for mode registration

**For mode development:**
- Study existing mode implementations: [StaticMode](lib/StaticMode/README.md), [WaveMode](lib/WaveMode/README.md), [BlinkMode](lib/BlinkMode/README.md)
- Reference [SensorService README](lib/SensorService/README.md) for donation detection
- See [SpeakerService README](lib/SpeakerService/README.md) for audio integration

## 📦 Project Structure

```
SpendenBox/
├── src/main.cpp              # Main application entry point
├── include/Config.h          # Hardware configuration
├── lib/
│   ├── AbstractMode/         # Base mode class → [README](lib/AbstractMode/README.md)
│   ├── Controller/           # Mode management → [README](lib/Controller/README.md)
│   ├── LightService/         # FastLED wrapper → [README](lib/LightService/README.md)
│   ├── SensorService/        # TCRT5000 handling → [README](lib/SensorService/README.md)
│   ├── SpeakerService/       # Audio service (mock) → [README](lib/SpeakerService/README.md)
│   ├── StaticMode/           # Breathing effect mode → [README](lib/StaticMode/README.md)
│   ├── WaveMode/             # Wave motion mode → [README](lib/WaveMode/README.md)
│   ├── BlinkMode/            # Random blink mode → [README](lib/BlinkMode/README.md)
│   ├── HalfMode/             # Half switching mode → [README](lib/HalfMode/README.md)
│   ├── CenterMode/           # Center expansion mode → [README](lib/CenterMode/README.md)
│   └── ChaseMode/            # Chase light mode → [README](lib/ChaseMode/README.md)
└── platformio.ini            # PlatformIO configuration
```

## 📞 Support

For questions or issues:
- Create GitHub Issues for bugs and feature requests
- Follow coding standards for contributions
- Include serial monitor output for debugging

---

*Professional modular LED controller system for donation boxes*  
*Created with ❤️ by Friedjof*
