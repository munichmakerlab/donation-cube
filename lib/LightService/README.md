# LightService

Professional WS2812B LED strip controller with FastLED integration for donation box animations.

## Overview

LightService provides a production-ready interface for controlling WS2812B LED strips using the FastLED library. It offers simplified LED management, brightness control, and hardware abstraction specifically designed for donation box applications.

## ✨ Key Features

- **💡 FastLED Integration**: Full WS2812B strip support
- **🎨 Color Management**: CRGB color space with professional control
- **🎚️ Brightness Control**: Global and individual LED brightness
- **⚡ Hardware Abstraction**: Platform-independent LED control
- **🔧 Auto Configuration**: Reads settings from Config.h
- **📱 Real-time Updates**: Immediate LED strip updates

## Hardware Requirements

**WS2812B LED Strip:**
- VCC → 5V (stable power supply recommended)
- GND → Ground
- DIN → GPIO pin (GPIO3 ESP32, GPIO12 ESP8266)
- **Important**: 5V power for stable operation
- **Note**: LED count configurable in Config.h (NUM_LEDS)

## Configuration

```cpp
// Config.h settings
#define NUM_LEDS            6       // Number of LEDs in strip
#define DATA_PIN            3       // ESP32: GPIO3, ESP8266: GPIO12  
#define LED_TYPE            WS2812B // FastLED LED type
#define MAX_BRIGHTNESS      255     // Maximum brightness level
```
```cpp
void setBrightness(uint8_t brightness)
```
**Purpose**: Set global brightness for all LEDs  
**Parameters**: `brightness` (0-255, where 0=off, 255=maximum)  
**Usage**: Call before setting colors for immediate effect

```cpp
uint8_t getBrightness() const
```
**Purpose**: Get current brightness setting  
**Returns**: Current brightness value (0-255)

### Color Control
```cpp
void setColor(const CRGB& color)
```
**Purpose**: Set all LEDs to the same color  
**Parameters**: `color` - CRGB color object (e.g., CRGB::White, CRGB::Black)  
**Usage**: Quick way to set uniform color across all LEDs

```cpp
void setLedColor(uint8_t index, const CRGB& color)
```
**Purpose**: Set individual LED color  
**Parameters**: 
- `index` - LED position (0 to NUM_LEDS-1)
- `color` - CRGB color object  
**Usage**: Primary method for creating patterns and animations

### Display Management
```cpp
void show()
```
**Purpose**: Update LED strip with current color data  
**Usage**: Call after setting colors to make changes visible  
**Note**: Automatically called by LightService methods

```cpp
void clear()
```
**Purpose**: Turn off all LEDs (set to black)  
**Usage**: Reset LED strip or clear previous patterns

## Usage Examples

### Basic Setup
```cpp
LightService* lightService = new LightService();
lightService->setup();
lightService->setBrightness(255); // Full brightness
```

### Set All LEDs White
```cpp
lightService->setColor(CRGB::White);
```

### Individual LED Control
```cpp
// Set first LED to bright white
lightService->setLedColor(0, CRGB::White);

// Set LED with custom brightness
CRGB dimWhite = CRGB::White;
dimWhite.fadeToBlackBy(128); // 50% brightness
lightService->setLedColor(5, dimWhite);
```

### Create Patterns
```cpp
// Turn off all LEDs first
lightService->clear();

// Create alternating pattern
for (int i = 0; i < NUM_LEDS; i++) {
    if (i % 2 == 0) {
        CRGB bright = CRGB::White;
        lightService->setLedColor(i, bright);
    } else {
        CRGB dim = CRGB::White;
        dim.fadeToBlackBy(200); // Very dim
        lightService->setLedColor(i, dim);
    }
}
```

### Brightness Variations
```cpp
// Full brightness
CRGB full = CRGB::White;

// 75% brightness  
CRGB bright = CRGB::White;
bright.fadeToBlackBy(64);

// 50% brightness
CRGB medium = CRGB::White;
medium.fadeToBlackBy(128);

// 25% brightness
CRGB dim = CRGB::White;
dim.fadeToBlackBy(192);

// Very dim (10%)
CRGB veryDim = CRGB::White;
veryDim.fadeToBlackBy(230);
```

### Animation Loop
```cpp
void updateAnimation() {
    // Clear previous frame
    lightService->clear();
    
    // Set new LED positions/colors
    for (int i = 0; i < patternWidth; i++) {
        int ledIndex = (currentPosition + i) % NUM_LEDS;
        CRGB color = CRGB::White;
        color.fadeToBlackBy(i * 40); // Gradient effect
        lightService->setLedColor(ledIndex, color);
    }
    
    // Changes are automatically displayed
}
```

## Color Management
The service uses CRGB objects for color representation:
- `CRGB::White` - Pure white
- `CRGB::Black` - LED off
- `CRGB(r, g, b)` - Custom RGB values
- `color.fadeToBlackBy(amount)` - Reduce brightness (0=no change, 255=black)

## Hardware Requirements
- WS2812B LED strip connected to DATA_PIN (defined in Config.h)
- Adequate power supply for LED strip
- Proper ground connection

## Dependencies
- FastLED library (≥3.6.0)
- Config.h (NUM_LEDS and DATA_PIN definitions)
