# 🎁 Modular Donation Box - Smart LED & Audio Controller

Professional LED animation system with 6 modes, audio feedback, automatic switching, sensor debouncing, and MQTT monitoring.

## 🎯 Overview

**Hardware:** ESP32-C3/ESP8266 + WS2812B LEDs + TCRT5000 sensor + DFPlayer Mini MP3 Player
**Modes:** Static Breathing | Wave Motion | Random Blink | Half Switch | Center Expansion | Chase Light  
**Features:** Donation detection with audio feedback, auto mode switching, WiFi/MQTT monitoring, robust startup, standalone mode

![Donation Cube](/media/donation-cube_small.png)_

### Wiring
```
LED Strip:    VCC→5V, GND→GND, DIN→GPIO3 (ESP32) / GPIO12 (ESP8266)
Sensor:       VCC→3.3V, GND→GND, OUT→GPIO2 (ESP32) / GPIO14 (ESP8266)  
DFPlayer:     VCC→5V, GND→GND, RX→GPIO4, TX→GPIO5 (ESP32) / RX→D1, TX→D2 (ESP8266)
Speaker:      Connect to DFPlayer Mini audio output
SD Card:      Insert into DFPlayer with MP3 files (001.mp3, 002.mp3, etc.)
```

### New Features ✨
- **🔊 Audio Feedback**: DFPlayer Mini integration with donation sounds
- **🛡️ Sensor Debouncing**: 500ms cooldown prevents false triggers
- **🔄 Robust Startup**: Smart restart mechanism with fallback modes
- **🎚️ Production Mode**: Serial debug can be disabled for standalone operation
- **⚡ Boot Loop Protection**: Automatic recovery from hardware issues

## � Configuration Options

### Production vs Debug Mode
```cpp
// include/Config.h
#define ENABLE_SERIAL_DEBUG 0  // Production: No serial dependency
#define ENABLE_SERIAL_DEBUG 1  // Debug: Full serial output
```

### Audio Configuration
```cpp
#define DFPLAYER_VOLUME     15              // Default volume (0-30)
#define DONATION_SOUND_COUNT 5              // Number of sound files
#define STARTUP_SOUND_FILE   1              // Startup sound
```

### Sensor Timing
```cpp
#define SENSOR_COOLDOWN_MS  500             // Debounce time between detections
```

## 📡 MQTT Topics & Standalone Mode

### WiFi/MQTT Mode (Default)
- `donation-box/{clientId}/donations` - Donation events with audio feedback
- `donation-box/{clientId}/mode` - Mode changes with timing info
- `donation-box/{clientId}/heartbeat` - System health (30s intervals)
- `donation-box/{clientId}/audio` - Audio system status

### Standalone Mode (WiFi-Free)
When WiFi is disabled during setup, the system operates as a pure LED controller:
- ✅ All LED modes work normally
- ✅ Donation detection with sensor debouncing
- ✅ Audio feedback on donations
- ✅ Automatic mode switching works
- ✅ Robust startup without dependencies
- ❌ No WiFi connection
- ❌ No MQTT monitoring
- ❌ No remote control

All events are logged to serial output for debugging (if ENABLE_SERIAL_DEBUG = 1). Perfect for offline installations or environments without WiFi access.

## 🚀 Installation & Quick Start

### 💾 One-Line Installation (Recommended)
Clone the repository and get ready to start:

```bash
curl -sSL https://raw.githubusercontent.com/munichmakerlab/donation-cube/main/scripts/install.sh > install.sh && chmod +x install.sh && ./install.sh
```
```bash
wget -qO- https://raw.githubusercontent.com/munichmakerlab/donation-cube/main/scripts/install.sh && chmod +x install.sh && ./install.sh
```

Or download and run manually:
```bash
wget https://raw.githubusercontent.com/munichmakerlab/donation-cube/main/scripts/install.sh
chmod +x install.sh
./install.sh
```

💡 **Note:** The one-line installation clones the repository and provides setup instructions. For the full interactive setup wizard, run `python3 scripts/setup.py` in the project directory.

### ⚡ Automated Setup (If already cloned)
```bash
python3 scripts/setup.py  # Complete workflow: config → build → flash → monitor
```

**Setup Options:**
- **Network Mode**: Configure WiFi and MQTT for full monitoring capabilities
- **Standalone Mode**: Skip WiFi/MQTT setup for offline-only LED controller operation

The setup script will ask if you want to enable WiFi/MQTT or run in standalone mode.

### 🔧 Manual Setup
```bash
pip install platformio && pio lib install
cp include/credentials.h.example include/credentials.h  # Edit WiFi/MQTT settings
# For standalone mode: Set ENABLE_WIFI = 0 in credentials.h
pio run -e esp32c3 --target upload  # or wemos_d1_mini for ESP8266
pio device monitor -e esp32c3
```

**Test:** LEDs breathe → wave hand → donation effect + mode switch → check MQTT broker (network mode) or serial output (standalone mode)

### 🗑️ Uninstallation
To completely remove the project and PlatformIO:

```bash
curl -sSL https://raw.githubusercontent.com/munichmakerlab/donation-cube/main/scripts/uninstall.sh | bash
```
```bash
wget -qO- https://raw.githubusercontent.com/munichmakerlab/donation-cube/main/scripts/uninstall.sh | bash
```

## 🎨 Creating Custom LED Modes

### Quick Implementation
1. **Create files:** `lib/YourMode/YourMode.hpp` & `YourMode.cpp`
2. **Inherit:** `class YourMode : public AbstractMode`
3. **Implement:** `setup()`, `loop()`, `donationTriggered()`
4. **Register:** Add to `src/main.cpp`

### Essential Pattern
```cpp
// YourMode.cpp
void YourMode::setup() {
    lightService->setup();
    effectDuration = 3000; // 3s donation effect
    // Initialize animation
}

void YourMode::loop() {
    // CRITICAL: Auto-end donation effect
    if (effectActive && millis() - effectStartTime >= effectDuration) {
        endDonationEffect();
    }
    // Your animation timing & logic
}

void YourMode::donationTriggered() {
    startDonationEffect(); // REQUIRED
    speakerService->playDonationSound(); // REQUIRED: Audio feedback
    // Modify animation using effectActive variable
}
```

### LED Control Examples
```cpp
// Single LED
lightService->setLedColor(index, CRGB::White);

// Brightness control
CRGB color = CRGB::White;
color.fadeToBlackBy(128); // 50% brightness (0=full, 255=off)

// Animation patterns
currentPosition = (currentPosition + 1) % NUM_LEDS; // Circular
float breath = (sin(millis() / 1000.0) + 1.0) / 2.0; // Breathing
```

**Study existing modes** in `lib/` for inspiration: StaticMode (breathing), WaveMode (movement), BlinkMode (random).

## 🔧 Troubleshooting

**LEDs not working:** Check 5V power, GPIO connection, ground, WS2812B compatibility  
**Sensor issues:** Adjust sensitivity pot, check 3.3V power, test positioning, verify 500ms cooldown  
**Audio issues:** Check DFPlayer connections, SD card with 001.mp3-005.mp3 files, 5V power  
**Boot loops:** Check serial debug setting (ENABLE_SERIAL_DEBUG), hardware connections  
**WiFi/MQTT issues:** Verify credentials, network access, or use standalone mode  
**Build errors:** `pio lib install FastLED DFRobotDFPlayerMini`, verify platformio.ini  

**After flashing works, but not after restart:** Set `ENABLE_SERIAL_DEBUG = 0` in Config.h for production use

## 🤝 Contributing

1. Fork → feature branch → implement mode → test → PR
2. Follow AbstractMode pattern, use white LEDs only
3. Include name/description/author/version metadata
4. Test with real hardware

## 📚 Architecture

**Services:** AbstractMode, Controller, LightService, SensorService, SpeakerService, MqttService  
**Modes:** Static, Wave, Blink, Half, Center, Chase (all with audio feedback)  
**Dependencies:** FastLED ≥3.6.0, DFRobotDFPlayerMini ≥1.0.6, PubSubClient (network mode only)

```
src/main.cpp → Controller → [6 Modes] → LightService → WS2812B
             → SensorService → TCRT5000 (with debouncing)
             → SpeakerService → DFPlayer Mini → Speaker/MP3
             → MqttService → WiFi/MQTT (optional)
```

**Key Features:**
- **Modular Design**: Each service is independent and testable
- **Robust Startup**: Boot loop protection and graceful degradation
- **Audio Integration**: Every mode plays donation sounds automatically  
- **Sensor Debouncing**: Prevents false triggers with 500ms cooldown
- **Production Ready**: Serial debug can be disabled for standalone operation

**Compilation Modes:**
- `ENABLE_WIFI = 1`: Full network functionality (default)
- `ENABLE_WIFI = 0`: Standalone LED controller (WiFi/MQTT disabled)
- `ENABLE_SERIAL_DEBUG = 1`: Full debug output (development)
- `ENABLE_SERIAL_DEBUG = 0`: No serial dependency (production)

---
*Professional LED controller for donation boxes* • *[Friedjof](https://github.com/Friedjof)* • *MIT License*
