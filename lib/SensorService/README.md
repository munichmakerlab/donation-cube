# SensorService

Professional donation detection using TCRT5000 infrared sensor with advanced debouncing and edge detection.

## Overview

The SensorService provides rock-solid donation detection for the donation box project. It features intelligent edge detection with 500ms debouncing to prevent false triggers from vibrations, electrical noise, or rapid movements.

## ✨ Key Features

- **🛡️ Advanced Debouncing**: 500ms cooldown prevents false triggers
- **⚡ Edge Detection**: Precise rising/falling edge detection
- **🔧 INPUT_PULLUP**: Professional GPIO configuration  
- **📊 State Monitoring**: Continuous sensor status tracking
- **🎯 Donation Focus**: Optimized for donation box use cases
- **🐛 Debug Support**: Detailed logging with cooldown tracking

## Hardware Requirements

**TCRT5000 Infrared Reflection Sensor:**
- VCC → 3.3V (stable power)
- GND → Ground  
- OUT → GPIO pin (GPIO2 ESP32, GPIO14 ESP8266)
- Sensitivity potentiometer for distance adjustment

**Optimal Placement:**
- 2-5cm detection distance
- Perpendicular to donation slot
- Protected from ambient light
- Stable mounting to prevent vibrations

## Public API

### Constructor
```cpp
SensorService(uint8_t pin)
```
**Purpose**: Initialize sensor service with specified GPIO pin  
**Parameters**: `pin` - GPIO pin connected to TCRT5000 output  
**Example**: `SensorService sensor(SENSOR_PIN);`

### Initialization
```cpp
void setup()
```
**Purpose**: Configure GPIO pin and initialize debouncing  
**Required**: Must call before using sensor functions  
**Action**: Sets INPUT_PULLUP mode and initializes cooldown timer

### Update Loop
```cpp
void loop()
```
**Purpose**: Process sensor readings with debouncing logic  
**Required**: Call in main loop for proper edge detection  
**Frequency**: Every main loop iteration for accurate timing

### Edge Detection (Primary Methods)
```cpp
bool risingEdge()
```
**Purpose**: Detect donation placement (HIGH→LOW transition)  
**Returns**: `true` if donation just detected (with 500ms cooldown)  
**Usage**: Primary method for triggering donation effects  
**Debouncing**: Automatic 500ms cooldown between detections

```cpp
bool fallingEdge()
```
**Purpose**: Detect donation removal (LOW→HIGH transition)  
**Returns**: `true` if object just removed (with 500ms cooldown)  
**Usage**: Optional cleanup or logging  
**Debouncing**: Same 500ms cooldown system
**Returns**: `true` if sensor just became inactive (falling edge detected)  
**Usage**: Detect when donation object is removed

### State Monitoring
```cpp
bool isActive()
```
**Purpose**: Check current sensor state  
**Returns**: `true` if sensor is currently detecting an object  
**Usage**: Continuous monitoring of sensor status

## Usage Examples

### Basic Setup
```cpp
SensorService* sensorService = new SensorService(SENSOR_PIN);
sensorService->setup();
```

### Donation Detection
```cpp
void loop() {
    // Update sensor state (required!)
    sensorService->loop();
    
    // Check for donation
    if (sensorService->risingEdge()) {
        Serial.println("*** DONATION DETECTED! ***");
        triggerDonationEffect();
    }
    
    // Check when object is removed
    if (sensorService->fallingEdge()) {
        Serial.println("Object removed from sensor");
    }
}
```

### Continuous Monitoring
```cpp
void checkSensorStatus() {
    sensorService->loop(); // Always call first
    
    if (sensorService->isActive()) {
        Serial.println("Sensor currently detecting object");
    } else {
        Serial.println("Sensor clear");
    }
}
```

### Complete Integration
```cpp
void setup() {
    sensorService->setup();
    Serial.println("Sensor service initialized");
}

void loop() {
    // Required sensor update
    sensorService->loop();
    
    // Rising edge = donation detected
    if (sensorService->risingEdge()) {
        Serial.println("*** DONATION DETECTED! ***");
        currentMode->donationTriggered();
    }
    
    // Falling edge = object removed
    if (sensorService->fallingEdge()) {
        Serial.println("Object removed from donation area");
    }
    
    // Continuous status (optional)
    static unsigned long lastStatusPrint = 0;
    if (millis() - lastStatusPrint > 5000) { // Every 5 seconds
        lastStatusPrint = millis();
        Serial.print("Sensor status: ");
        Serial.println(sensorService->isActive() ? "ACTIVE" : "INACTIVE");
    }
}
```

## Sensor Behavior
- **HIGH state**: No object detected (sensor clear)
- **LOW state**: Object detected (donation present)
- **Rising edge**: Transition HIGH→LOW (donation placed)
- **Falling edge**: Transition LOW→HIGH (donation removed)

## Hardware Configuration
### TCRT5000 Wiring
```
TCRT5000    →  ESP32-C3    | Description
------------|--------------|-------------
VCC         →  3.3V        | Power supply
GND         →  GND         | Ground
OUT         →  SENSOR_PIN  | Digital output
```

### Sensor Positioning
- **Distance**: 2-10mm from detection surface
- **Angle**: Perpendicular to target surface
- **Environment**: Avoid direct sunlight interference
- **Target**: Works best with non-reflective objects

## Technical Details
- **Pull-up**: Uses INPUT_PULLUP for stable readings
- **Debouncing**: Built-in state change filtering
- **Edge Detection**: Reliable transition detection
- **Update Rate**: Call `loop()` every main loop iteration

## Troubleshooting

### Sensor Not Detecting
- Check 3.3V power supply
- Verify wiring connections
- Adjust sensor distance (2-10mm optimal)
- Test with different objects

### False Triggers
- Ensure stable mounting
- Check for vibrations
- Verify proper debouncing by calling `loop()`
- Shield from external light sources

### Sensitivity Issues
- Some TCRT5000 modules have sensitivity potentiometer
- Adjust sensitivity screw if available
- Test with various object materials

## Dependencies
- Config.h (SENSOR_PIN definition)
- Arduino.h (GPIO functions)
