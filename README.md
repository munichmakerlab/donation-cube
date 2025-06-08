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

### Mode Development
To create a new mode:

1. **Create mode files**:
```cpp
// lib/NewMode/NewMode.hpp
class NewMode : public AbstractMode {
public:
    NewMode(LightService* lightService, SpeakerService* speakerService);
    void setup() override;
    void loop() override;
    void donationTriggered() override;
};

// lib/NewMode/NewMode.cpp
NewMode::NewMode(LightService* lightService, SpeakerService* speakerService) 
    : AbstractMode(lightService, speakerService,
                  "Your Mode Name",
                  "Mode description", 
                  "Your Name",
                  "v1.0.0") {
}
```

2. **Register in main.cpp**:
```cpp
#include "NewMode.hpp"
// ...
NewMode* newMode = new NewMode(lightService, speakerService);
controller->addMode(newMode);
```

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

## 📦 Project Structure

```
SpendenBox/
├── src/main.cpp              # Main application entry point
├── include/Config.h          # Hardware configuration
├── lib/
│   ├── AbstractMode/         # Base mode class
│   ├── Controller/           # Mode management
│   ├── LightService/         # FastLED wrapper
│   ├── SensorService/        # TCRT5000 handling
│   ├── SpeakerService/       # Audio service (mock)
│   ├── StaticMode/           # Breathing effect mode
│   ├── WaveMode/             # Wave motion mode
│   ├── BlinkMode/            # Random blink mode
│   ├── HalfMode/             # Half switching mode
│   ├── CenterMode/           # Center expansion mode
│   └── ChaseMode/            # Chase light mode
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
