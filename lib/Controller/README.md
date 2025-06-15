# Controller

Central coordination system managing LED modes, sensor integration, and audio feedback for the donation box.

## Overview

The Controller acts as the brain of the donation box system, orchestrating mode transitions, handling donation detection, and ensuring seamless audio-visual feedback. It provides intelligent mode management with automatic progression after donation effects.

## ✨ Key Features

- **🎛️ Mode Management**: Automatic and manual mode switching
- **📡 Sensor Integration**: Real-time donation detection with debouncing
- **🎵 Audio Coordination**: Integrated audio feedback via SpeakerService
- **⏱️ Effect Timing**: Precise 3-second donation effects
- **🔄 Auto Progression**: Automatic mode switching after donations
- **📊 State Tracking**: Mode lifecycle and timing management
- **🎯 Professional Control**: Smooth transitions and coordination

## Architecture

```
Controller
├── SensorService (donation detection)
├── SpeakerService (audio feedback)  
├── Mode Array [StaticMode, WaveMode, BlinkMode, ...]
└── Current Active Mode
```

## Core Functionality

### Mode Lifecycle Management
1. **Registration**: Add modes during setup
2. **Activation**: Setup and start mode animation
3. **Runtime**: Continuous loop execution
4. **Effect Handling**: Donation effect coordination
5. **Deactivation**: Clean shutdown and mode switch

## Public Functions

### Constructor
```cpp
Controller(SensorService* sensorService)
```
**Purpose**: Initialize controller with sensor service  
**Parameters**: `sensorService` - Configured sensor service for donation detection  
**Usage**: Create with initialized SensorService instance

### Mode Management
```cpp
void addMode(AbstractMode* mode)
```
**Purpose**: Register a new LED mode with the controller  
**Parameters**: `mode` - Pointer to initialized mode instance  
**Usage**: Add all available modes during setup  
**Limit**: Maximum MAX_MODES (10) modes supported

```cpp
void switchToNextMode()
```
**Purpose**: Manually switch to next mode in sequence  
**Usage**: Public method for external mode switching  
**Behavior**: Deactivates current mode, activates next mode

### Main Functions
```cpp
void setup()
```
**Purpose**: Initialize controller and activate first mode  
**Must call**: After adding all modes, before main loop  
**Action**: Sets up first mode and prints mode information

```cpp
void loop()
```
**Purpose**: Main controller update loop  
**Must call**: Every main loop iteration  
**Actions**: 
- Updates sensor service
- Handles donation detection
- Manages mode lifecycle
- Triggers automatic mode switching

## Usage Examples

### Basic Setup
```cpp
// Create services
SensorService* sensorService = new SensorService(SENSOR_PIN);
LightService* lightService = new LightService();
SpeakerService* speakerService = new SpeakerService();

// Create controller
Controller* controller = new Controller(sensorService);

// Create and add modes
StaticMode* staticMode = new StaticMode(lightService, speakerService);
WaveMode* waveMode = new WaveMode(lightService, speakerService);
BlinkMode* blinkMode = new BlinkMode(lightService, speakerService);

controller->addMode(staticMode);
controller->addMode(waveMode);
controller->addMode(blinkMode);

// Initialize everything
sensorService->setup();
lightService->setup();
speakerService->setup();
controller->setup();
```

### Main Loop Integration
```cpp
void loop() {
    // Controller handles everything automatically
    controller->loop();
    
    // Optional: Manual mode switching
    if (buttonPressed()) {
        controller->switchToNextMode();
    }
}
```

### Complete System Example
```cpp
// Global variables
Controller* controller;
SensorService* sensorService;
LightService* lightService;
SpeakerService* speakerService;

void setup() {
    Serial.begin(115200);
    Serial.println("[INFO] Starting modular donation box system...");
    
    // Initialize services
    sensorService = new SensorService(SENSOR_PIN);
    lightService = new LightService();
    speakerService = new SpeakerService();
    
    // Initialize controller
    controller = new Controller(sensorService);
    Serial.println("[INFO] Controller setup started");
    
    // Create and register all modes
    StaticMode* staticMode = new StaticMode(lightService, speakerService);
    WaveMode* waveMode = new WaveMode(lightService, speakerService);
    BlinkMode* blinkMode = new BlinkMode(lightService, speakerService);
    HalfMode* halfMode = new HalfMode(lightService, speakerService);
    CenterMode* centerMode = new CenterMode(lightService, speakerService);
    ChaseMode* chaseMode = new ChaseMode(lightService, speakerService);
    
    controller->addMode(staticMode);
    controller->addMode(waveMode);
    controller->addMode(blinkMode);
    controller->addMode(halfMode);
    controller->addMode(centerMode);
    controller->addMode(chaseMode);
    
    // Setup all services
    sensorService->setup();
    lightService->setup();
    speakerService->setup();
    
    // Setup controller (activates first mode)
    controller->setup();
    
    Serial.println("[INFO] Setup complete. Donation box ready!");
}

void loop() {
    // Controller manages everything
    controller->loop();
}
```

## Automatic Mode Switching
The controller automatically switches modes when:
1. **Donation detected**: Sensor rising edge triggers `donationTriggered()`
2. **Effect completes**: Donation effect duration expires
3. **Mode deactivates**: Current mode automatically deactivates
4. **Next mode activates**: Controller switches to next mode in sequence

### Mode Switching Sequence
```
Current Mode Running
        ↓
Donation Detected (sensor rising edge)
        ↓
Mode's donationTriggered() called
        ↓
Donation effect runs (2-5 seconds)
        ↓
Effect duration expires
        ↓
Mode automatically deactivates
        ↓
Controller switches to next mode
        ↓
New mode setup() and activation
        ↓
New Mode Running
```

## Serial Output
The controller provides detailed logging:
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

## Mode Capacity
- **Maximum modes**: 10 (defined by MAX_MODES)
- **Current usage**: 6 modes (Static, Wave, Blink, Half, Center, Chase)
- **Available slots**: 4 additional modes can be added

## Error Handling
- **Mode limit**: Ignores additional modes beyond MAX_MODES
- **Null checks**: Handles null mode pointers safely
- **Index wrapping**: Mode switching wraps around (last→first)

## Dependencies
- AbstractMode (base class for all modes)
- SensorService (donation detection)
- Config.h (constants and definitions)
