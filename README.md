# 🎁 Donation Cube - LED Controller

An automatic LED animation and sound system for donation boxes that triggers spectacular light effects and sound effects with every donation.

## 📋 Overview

This project implements an intelligent controller for a donation box that:
- Shows continuous LED color rotation in standby mode
- Starts a spectacular 800ms animation when a donation is detected
- Plays synchronous sound effects via a DFPlayer Mini
- Implements a 2-second debounce against sensor flickering

## 🔧 Hardware Components

| Component | Description | Purpose |
|-----------|-------------|---------|
| **ESP8266 (Wemos D1 Mini)** | Microcontroller | Main control |
| **WS2812B LED-Strip** | 6 addressable LEDs | Light effects |
| **TCRT5000 Reflection Sensor** | Infrared sensor | Donation detection |
| **DFPlayer Mini** | MP3 module | Sound playback |
| **MicroSD Card** | Storage (FAT32) | MP3 files |
| **Speaker** | Audio output | Sound effects |

## 🔌 Pin Configuration

| ESP8266 Pin | Function | Component | Description |
|-------------|----------|-----------|-------------|
| **D2** | `SENSOR_PIN` | TCRT5000 | Donation sensor (INPUT_PULLUP) |
| **D3** | `DATA_PIN` | WS2812B | LED strip data line |
| **D5** | `DFPLAYER_RX` | DFPlayer Mini TX | Serial communication |
| **D6** | `DFPLAYER_TX` | DFPlayer Mini RX | Serial communication |

## 🔧 Wiring

### DFPlayer Mini
```
DFPlayer  →  ESP8266     | Description
----------|--------------|-------------
VCC       →  5V/3.3V     | Power supply
GND       →  GND         | Ground
TX        →  D5          | DFPlayer output → ESP8266 input
RX        →  D6          | DFPlayer input ← ESP8266 output
SPK+      →  Speaker+    | Speaker positive
SPK-      →  Speaker-    | Speaker negative
```

### LED-Strip (WS2812B)
```
LED-Strip →  ESP8266     | Description
----------|--------------|-------------
VCC       →  5V          | Power supply
GND       →  GND         | Ground
DIN       →  D3          | Data line
```

### TCRT5000 Sensor
```
Sensor    →  ESP8266     | Description
----------|--------------|-------------
VCC       →  3.3V        | Power supply
GND       →  GND         | Ground
OUT       →  D2          | Digital output
```

## 💾 MicroSD Card Setup

### Format and Structure
- **Format**: FAT32
- **Files in root directory**:
  ```
  /
  ├── 001.mp3  → Main sound for donation detection
  ├── 002.mp3  → Optional: Additional sounds
  └── 003.mp3  → Optional: Additional sounds
  ```

### Audio Specifications
- **Supported formats**: MP3, WAV
- **Bitrate**: 8–320 kbps
- **Recommended length**: 2-5 seconds for optimal user experience

## 🎨 Animation Sequence

| Phase | Duration | Effect | Description |
|-------|----------|--------|-------------|
| **1** | 200ms | White Flash | All LEDs light up white |
| **2** | 200ms | Rainbow Wave | Color wave across all LEDs |
| **3** | 400ms | Fast Color Rotation | All LEDs rotate quickly through colors |
| **End** | - | Normal Rotation | Back to continuous color rotation |

**Total**: 800ms animation + sound effect  
**Debounce**: 2 seconds pause between animations

## 🚀 Installation & Setup

### 1. Development Environment
```bash
# Install PlatformIO
pip install platformio

# Clone project
git clone git@github.com:munichmakerlab/donation-cube.git
cd donation-cube

# Install dependencies
pio lib install
```

### 2. Compile and Upload
```bash
# Compile code
pio run

# Upload to ESP8266
pio run --target upload

# Open serial monitor
pio device monitor --baud 115200
```

### 3. Hardware Tests
1. **LED Test**: After startup, LEDs should show continuous color rotation
2. **Sensor Test**: Move object in front of TCRT5000
3. **Sound Test**: Sound should play when sensor is triggered

## 📚 Used Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| [FastLED](https://github.com/FastLED/FastLED) | ≥3.6.0 | WS2812B LED strip control |
| [DFRobotDFPlayerMini](https://github.com/DFRobot/DFRobotDFPlayerMini) | ≥1.0.5 | MP3 player control |
| [SoftwareSerial](https://www.arduino.cc/en/Reference/SoftwareSerial) | Arduino Core | Serial communication |

## 🛠️ Configuration

### LED Settings
```cpp
#define NUM_LEDS 6           // Number of LEDs in strip
#define BRIGHTNESS 255       // Brightness (0-255)
#define SATURATION 255       // Color saturation (0-255)
```

### Timing Parameters
```cpp
#define DEBOUNCE_TIME 2000      // Debounce time in ms
#define ANIMATION_DURATION 800  // Animation duration in ms
```

### Audio Settings
```cpp
dfPlayer.volume(25);        // Volume (0-30)
dfPlayer.play(1);           // Play file 001.mp3
```

## 🔍 Troubleshooting

### DFPlayer Mini Issues
```
ERROR: DFPlayer Mini not connected!
Check:
1. Wiring (RX->D5, TX->D6, VCC->5V, GND->GND)
2. MicroSD card inserted
3. MP3 files on SD card
```

**Solutions**:
- Check wiring
- Format SD card (FAT32)
- Name MP3 files correctly (001.mp3, 002.mp3, ...)
- Check power supply

### LED Issues
- **LEDs not lighting**: Check power supply and data line
- **Wrong colors**: Adjust `FastLED.setCorrection()`
- **Flickering**: Stabilize power supply

### Sensor Issues
- **No detection**: Adjust sensor sensitivity
- **Constant trigger**: Increase debounce time
- **Unreliable**: Optimize sensor position

## 📖 Serial Monitor Output

```
+=============================================+
|        SPENDENBOX LED CONTROLLER            |
+=============================================+
Initialisiere DFPlayer Mini...
DFPlayer Mini erfolgreich initialisiert!
SpendenBox LED Controller bereit!
Warte auf Spenden...

*** SPENDE ERKANNT! Animation und Sound starten! ***
Animation beendet - zurück zur Farbrotation
```

## 🤝 Contributing

Improvements and extensions are welcome!

1. Create a fork of the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Commit changes (`git commit -m 'Add new feature'`)
4. Push branch (`git push origin feature/new-feature`)
5. Create a pull request

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 👥 Authors

- **Munich Maker Lab** - *Initial work* - [munichmakerlab](https://github.com/munichmakerlab)

## 🎯 Roadmap

- [ ] Web interface for configuration
- [ ] Multiple animation modes
- [ ] MQTT integration
- [ ] WiFi configuration via captive portal
- [ ] Statistics on donation frequency
- [ ] Different sounds for different donation values

## 📞 Support

For questions or issues:
- GitHub Issues: [Issues](https://github.com/munichmakerlab/donation-cube/issues)
- Munich Maker Lab: [Website](https://munichmakerlab.de)

---

*Created with ❤️ for Munich Maker Lab*
