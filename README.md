# 🎁 Modular Donation Box - LED Controller

Professional LED animation system with 6 modes, automatic switching, and MQTT monitoring.

## 🎯 Overview

**Hardware:** ESP32-C3/ESP8266 + WS2812B LEDs + TCRT5000 sensor +  [Arduino DFPlayer](https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299)
**Modes:** Static Breathing | Wave Motion | Random Blink | Half Switch | Center Expansion | Chase Light  
**Features:** Donation detection, auto mode switching, WiFi/MQTT monitoring, standalone mode

### Wiring
```
LED Strip: VCC→5V, GND→GND, DIN→GPIO8
Sensor:    VCC→3.3V, GND→GND, OUT→GPIO2
```

## 📡 MQTT Topics & Standalone Mode

### WiFi/MQTT Mode (Default)
- `donation-box/{clientId}/donations` - Events  
- `donation-box/{clientId}/mode` - Mode changes  
- `donation-box/{clientId}/heartbeat` - Health (30s)

### Standalone Mode (WiFi-Free)
When WiFi is disabled during setup, the system operates as a pure LED controller:
- ✅ All LED modes work normally
- ✅ Donation detection works
- ✅ Automatic mode switching works
- ✅ Sensor functionality works
- ❌ No WiFi connection
- ❌ No MQTT monitoring
- ❌ No remote control

All events are logged to serial output for debugging. Perfect for offline installations or environments without WiFi access.

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
    // Optional: speakerService->playSound()
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

**LEDs not working:** Check 5V power, GPIO8 connection, ground, WS2812B compatibility  
**Sensor issues:** Adjust sensitivity pot, check 3.3V power, test positioning  
**WiFi/MQTT issues:** Verify credentials, network access, or use standalone mode  
**Build errors:** `pio lib install FastLED`, verify platformio.ini, check includes

## 🤝 Contributing

1. Fork → feature branch → implement mode → test → PR
2. Follow AbstractMode pattern, use white LEDs only
3. Include name/description/author/version metadata
4. Test with real hardware

## 📚 Architecture

**Services:** AbstractMode, Controller, LightService, SensorService, SpeakerService, MqttService  
**Modes:** Static, Wave, Blink, Half, Center, Chase  
**Dependencies:** FastLED ≥3.6.0, PubSubClient (network mode only)

```
src/main.cpp → Controller → [6 Modes] → LightService → WS2812B
             → SensorService → TCRT5000
             → MqttService → WiFi/MQTT (optional)
```

**Compilation Modes:**
- `ENABLE_WIFI = 1`: Full network functionality (default)
- `ENABLE_WIFI = 0`: Standalone LED controller (WiFi/MQTT disabled)

---
*Professional LED controller for donation boxes* • *[Friedjof](https://github.com/Friedjof)* • *MIT License*
