# SensorService Library

## Overview
SensorService provides donation detection using a TCRT5000 infrared reflection sensor. It features edge detection, debouncing, and reliable state monitoring for triggering donation effects.

## Purpose
- **Donation detection** using TCRT5000 infrared sensor
- **Edge detection** for rising and falling sensor transitions
- **Debouncing** to prevent false triggers
- **State monitoring** for continuous sensor status checking
- **Professional sensor handling** with INPUT_PULLUP configuration

## Public Functions

### Constructor
```cpp
SensorService(uint8_t pin)
```
**Purpose**: Initialize sensor service with specified pin  
**Parameters**: `pin` - GPIO pin connected to sensor output  
**Usage**: Pass SENSOR_PIN from Config.h during construction

### Setup and Initialization
```cpp
void setup()
```
**Purpose**: Configure sensor pin as INPUT_PULLUP  
**Must call**: Before using sensor functions  
**Action**: Sets up GPIO pin for sensor reading

### Main Update
```cpp
void loop()
```
**Purpose**: Update sensor state and handle debouncing  
**Must call**: In main loop for proper edge detection  
**Frequency**: Call every loop iteration for accurate detection

### Edge Detection
```cpp
bool risingEdge()
```
**Purpose**: Detect transition from inactive to active (donation detected)  
**Returns**: `true` if sensor just became active (rising edge detected)  
**Usage**: Primary method for donation detection

```cpp
bool fallingEdge()
```
**Purpose**: Detect transition from active to inactive (object removed)  
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
