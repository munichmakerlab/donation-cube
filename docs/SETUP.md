# 🎁 SpendenBox - Complete Setup Guide

Welcome to your SpendenBox! This comprehensive guide will help you set up WiFi, MQTT, and flash your device in just a few minutes.

## 🚀 Quick Installation (One-Line Install)

The fastest way to get started is using our automated installer:

```bash
# Option 1: Using curl
curl -sSL https://raw.githubusercontent.com/friedjof/SpendenBox/main/scripts/install.sh > install.sh && chmod +x install.sh && ./install.sh

# Option 2: Using wget
wget -qO- https://raw.githubusercontent.com/friedjof/SpendenBox/main/scripts/install.sh && chmod +x install.sh && ./install.sh
```

This will:
- ✅ Clone the repository
- ✅ Install dependencies (Python, PlatformIO)
- ✅ Run the interactive setup wizard
- ✅ Guide you through the complete configuration

## 📦 Manual Installation

If you prefer to install manually:

```bash
# Clone the repository
git clone https://github.com/friedjof/SpendenBox.git
cd SpendenBox

# Run the setup script
python3 scripts/setup.py
```

## 🔧 Setup Options

### Interactive Wizard Mode (Default)
```bash
python3 scripts/setup.py
```
The wizard will guide you through:
1. **Dependency Installation**: Automatic PlatformIO setup using official installer
2. **WiFi Configuration**: Network credentials and settings
3. **MQTT Setup**: Broker configuration for remote monitoring
4. **Device Detection**: Automatic ESP32/ESP8266 detection
5. **Build & Flash**: Compile and upload firmware
6. **Serial Monitor**: Real-time debugging and monitoring

### Command-Line Interface
For automation or expert users, use direct commands:

```bash
# Show available devices
python3 scripts/setup.py --devices

# Configure WiFi and MQTT only
python3 scripts/setup.py --config

# Build firmware only
python3 scripts/setup.py --build

# Flash to device only
python3 scripts/setup.py --flash

# Start serial monitor only
python3 scripts/setup.py --monitor

# Complete setup (config + build + flash + monitor)
python3 scripts/setup.py --full

# Show help
python3 scripts/setup.py --help
```

### Non-Interactive Mode
The setup works automatically in non-interactive environments (CI/CD, remote servers):
- Uses example credentials as fallbacks
- Selects first available device automatically
- No user input required

## 🛠️ Supported Hardware

### Microcontrollers
- **ESP32 Development Board** (esp32_dev)
- **ESP32-S3** (esp32_s3)
- **ESP32-C3** (esp32c3) - including Seeed XIAO ESP32C3
- **ESP8266 NodeMCU v2** (nodemcuv2)
- **Wemos D1 Mini** (wemos_d1_mini)

### Components
- **WS2812B LED Strip** (6 LEDs)
- **TCRT5000 Infrared Sensor**
- **Power Supply** (5V or 3.3V depending on board)

### Device Detection
The setup automatically detects:
- Standard ESP32/ESP8266 boards
- ESP32 JTAG debug units
- Filters out virtual serial ports
- Shows device descriptions and VID/PID information

## 📡 MQTT Integration

MQTT enables remote monitoring of your SpendenBox:
- 📊 **Real-time donation notifications**
- 📱 **Status updates to phone/computer**
- 🌐 **Smart home system integration**
- 📈 **Donation statistics collection**
- 🔔 **Custom alerting and automation**

### MQTT Topics
```
spendenbox/donations/count    # Total donation count
spendenbox/donations/new      # New donation events
spendenbox/status/online      # Device online status
spendenbox/status/wifi        # WiFi connection status
spendenbox/debug/logs         # Debug information
```

## 🔒 Security & Privacy

- ✅ All credentials stay **only on your computer**
- ✅ Automatic **Git protection** prevents accidental uploads
- ✅ **No cloud connection** required for setup
- ✅ **Local network only** - no internet dependencies
- ✅ **Open source** - audit the code yourself

## ⚡ Expert Quick Start

If you know what you're doing:

```bash
# Manual configuration
cp include/credentials.h.example include/credentials.h
nano include/credentials.h  # Edit with your credentials

# Build and upload using PlatformIO
pio run --target upload

# Monitor serial output
pio device monitor
```

## 🗑️ Uninstallation

To completely remove SpendenBox and PlatformIO:

```bash
# Run the uninstall script
./scripts/uninstall.sh
```

This removes:
- Project directory
- PlatformIO installation (pip, pipx, manual)
- Cache and configuration files
- All related dependencies

## 🐛 Troubleshooting

### Common Issues

**Device not detected:**
```bash
# List all serial devices
python3 scripts/setup.py --devices

# Check USB connections and drivers
lsusb  # Linux/macOS
```

**PlatformIO installation fails:**
```bash
# Manual PlatformIO installation
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py
python3 get-platformio.py
```

**Build errors:**
```bash
# Clean build directory
pio run --target clean

# Update PlatformIO
pio upgrade
pio platform update
```

**Upload fails:**
```bash
# Check device permissions (Linux)
sudo usermod -a -G dialout $USER
# Logout and login again

# Reset device before upload
# Hold BOOT button while connecting USB
```

**Serial monitor shows garbage:**
- Check baud rate (115200)
- Verify correct device selection
- Try different USB cable/port

### Debug Mode
```bash
# Enable verbose output
export PLATFORMIO_LOG_LEVEL=DEBUG
python3 scripts/setup.py --full
```

### Manual Firmware Upload
```bash
# Using esptool (if PlatformIO fails)
pip install esptool
esptool.py --port /dev/ttyUSB0 write_flash 0x1000 .pio/build/esp32_dev/firmware.bin
```

## 📞 Getting Help

1. 📋 **Check serial output**: `pio device monitor`
2. 🔍 **Read documentation**: Browse the `docs/` folder
3. 🐛 **Report bugs**: Create a GitHub issue
4. 💬 **Ask questions**: Use GitHub discussions
5. 📧 **Contact**: Open an issue for support

### Useful Commands
```bash
# Show device info
pio device list

# Show build information
pio run --verbose

# Update all dependencies
make update

# Run tests
make test

# Clean everything
make clean
```

## 🔄 Updates

To update your SpendenBox installation:

```bash
# Pull latest changes
git pull origin main

# Update dependencies
pio upgrade
pio platform update

# Rebuild and reflash
python3 scripts/setup.py --full
```

---

**Enjoy your SpendenBox! 🎉**

*For more detailed MQTT configuration, see [MQTT_GUIDE.md](MQTT_GUIDE.md)*
