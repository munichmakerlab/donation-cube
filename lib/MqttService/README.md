# MqttService Library

## Overview
MqttService provides MQTT connectivity for the donation box system, enabling remote logging, donation tracking, and system monitoring. The service handles WiFi connection management, MQTT broker communication, and automatic reconnection.

## Purpose
- **Remote logging** with structured JSON messages
- **Donation tracking** with timestamp and mode information  
- **System monitoring** via heartbeat and status messages
- **Mode change notifications** for analytics and debugging
- **Automatic reconnection** for reliable connectivity
- **Network resilience** with connection state management

## Features
- **WiFi Management**: Automatic connection and reconnection
- **MQTT Communication**: Structured JSON message publishing
- **Topic Organization**: Hierarchical topic structure for easy filtering
- **Connection Monitoring**: Real-time status reporting
- **Heartbeat System**: Regular alive signals with system metrics
- **Error Handling**: Graceful degradation when disconnected

## Public Functions

### Constructor
```cpp
MqttService(const char* ssid, const char* password, 
            const char* server, int port = 1883,
            const char* clientId = "donation-box",
            const char* user = nullptr, const char* pass = nullptr)
```
**Purpose**: Initialize MQTT service with network credentials  
**Parameters**: 
- WiFi credentials (SSID, password)
- MQTT broker settings (server, port, client ID)
- Optional MQTT authentication (user, password)
- Automatic topic generation based on client ID

### Core Service Methods

```cpp
void setup()
```
**Purpose**: Initialize WiFi and MQTT connections  
**Actions**:
- Establishes WiFi connection with configured credentials
- Connects to MQTT broker with authentication if provided
- Sets up topic hierarchy based on client ID
- Sends initial "online" status and startup log message

```cpp
void loop()
```
**Purpose**: Maintain network connections and handle communication  
**Behavior**:
- Monitors WiFi connection status and reconnects if needed
- Maintains MQTT broker connection with automatic retry
- Processes MQTT client messages and keepalive
- Sends periodic heartbeat messages with system metrics
- Implements connection retry with 5-second intervals

### Publishing Methods

```cpp
void donation(const String& mode, int amount = 1)
```
**Purpose**: Report donation events with context  
**Parameters**: Current LED mode name, optional donation amount  
**Message Format**:
```json
{
  "timestamp": "12:34:56",
  "mode": "StaticMode",
  "amount": 1,
  "event": "donation"
}
```
**Topic**: `donation-box/{clientId}/donations`

```cpp
void logInfo(const String& message)
void logWarning(const String& message)  
void logError(const String& message)
```
**Purpose**: Send structured log messages with severity levels  
**Message Format**:
```json
{
  "timestamp": "12:34:56",
  "level": "INFO|WARNING|ERROR",
  "message": "System startup complete"
}
```
**Topic**: `donation-box/{clientId}/logs`

```cpp
void modeChanged(const String& fromMode, const String& toMode)
```
**Purpose**: Track LED mode transitions for analytics  
**Message Format**:
```json
{
  "timestamp": "12:34:56",
  "from_mode": "StaticMode",
  "to_mode": "WaveMode",
  "event": "mode_change"
}
```
**Topic**: `donation-box/{clientId}/mode`

```cpp
void systemStatus(const String& status)
```
**Purpose**: Send comprehensive system status updates  
**Message Format**:
```json
{
  "timestamp": "12:34:56",
  "status": "online",
  "wifi_connected": true,
  "mqtt_connected": true,
  "free_heap": 45632,
  "uptime": 123456
}
```
**Topic**: `donation-box/{clientId}/status`

### Status Methods

```cpp
bool isConnected() const
```
**Purpose**: Check if MQTT broker connection is active  
**Returns**: true if connected to MQTT broker

```cpp
bool isWiFiConnected() const
```
**Purpose**: Check if WiFi connection is active  
**Returns**: true if connected to WiFi network

```cpp
String getConnectionStatus() const
```
**Purpose**: Get human-readable connection status  
**Returns**: Descriptive status string for debugging

### Configuration Methods

```cpp
void setBaseTopic(const String& topic)
```
**Purpose**: Customize MQTT topic hierarchy  
**Effect**: Updates all topic paths to use new base topic  
**Default**: `donation-box/{clientId}`

## Topic Structure

The service uses a hierarchical topic structure for organized message routing:

```
donation-box/{clientId}/
├── donations     # Donation events with mode context
├── logs          # System log messages (INFO/WARNING/ERROR)  
├── status        # System status and health information
├── mode          # LED mode change notifications
└── heartbeat     # Periodic alive signals with metrics
```

## Message Formats

All messages use structured JSON format with consistent field naming:

### Common Fields
- **timestamp**: Simple time format (HH:MM:SS) using system uptime
- **event**: Event type identifier for message categorization

### Donation Events
```json
{
  "timestamp": "12:34:56",
  "mode": "current_mode_name", 
  "amount": 1,
  "event": "donation"
}
```

### Log Messages  
```json
{
  "timestamp": "12:34:56",
  "level": "INFO|WARNING|ERROR",
  "message": "descriptive_message"
}
```

### System Status
```json
{
  "timestamp": "12:34:56",
  "status": "online|offline|reconnected",
  "wifi_connected": true,
  "mqtt_connected": true, 
  "free_heap": 45632,
  "uptime": 123456
}
```

### Heartbeat
```json
{
  "timestamp": "12:34:56",
  "event": "heartbeat",
  "uptime": 123456,
  "free_heap": 45632
}
```

## Connection Management

### WiFi Handling
- **Auto-connect**: Attempts connection on startup and after disconnection
- **Retry logic**: 5-second intervals between reconnection attempts  
- **Status monitoring**: Continuous WiFi status checking
- **Timeout**: 10-second timeout for initial connection

### MQTT Handling
- **Broker connection**: Automatic connection with credentials if provided
- **Reconnection**: Automatic retry with exponential backoff
- **Keepalive**: Built-in MQTT keepalive and message processing
- **State management**: Clean connection state tracking

### Error Handling
- **Graceful degradation**: System continues without MQTT if disconnected
- **Silent failures**: Log messages fail silently to prevent spam
- **Connection retry**: Persistent reconnection attempts
- **Status reporting**: Clear connection status for debugging

## Technical Details

### Dependencies
- **WiFi.h**: ESP32/ESP8266 WiFi functionality
- **PubSubClient**: MQTT client library for Arduino
- **ArduinoJson**: JSON message formatting (implicit)

### Memory Management
- **Static allocation**: No dynamic memory allocation during runtime
- **String optimization**: Efficient string handling for topics and messages
- **Heap monitoring**: Free heap reporting in status messages

### Timing
- **Reconnection interval**: 5 seconds between retry attempts
- **Heartbeat interval**: 30 seconds between heartbeat messages
- **Connection timeout**: 10 seconds for WiFi connection attempts

### Platform Support
- **ESP32**: Full support with WiFi.h
- **ESP8266**: Compatible with ESP8266WiFi.h
- **Arduino framework**: Standard Arduino libraries

## Usage Example

```cpp
#include "MqttService.hpp"

// Initialize MQTT service
MqttService* mqttService = new MqttService(
    "your-wifi-ssid",           // WiFi SSID
    "your-wifi-password",       // WiFi password  
    "mqtt.broker.com",          // MQTT broker
    1883,                       // MQTT port
    "donation-box-01",          // Client ID
    "mqtt-user",                // MQTT username (optional)
    "mqtt-pass"                 // MQTT password (optional)
);

void setup() {
    // Setup MQTT service
    mqttService->setup();
    
    // Custom topic base (optional)
    mqttService->setBaseTopic("custom/donation-boxes/box-01");
}

void loop() {
    // Maintain connections
    mqttService->loop();
    
    // Report donation
    if (donationDetected) {
        mqttService->donation("StaticMode");
        mqttService->logInfo("Donation detected in StaticMode");
    }
    
    // Report mode change
    if (modeChanged) {
        mqttService->modeChanged("StaticMode", "WaveMode");
    }
    
    // Check connection status
    if (!mqttService->isConnected()) {
        Serial.println("MQTT disconnected: " + mqttService->getConnectionStatus());
    }
}
```

## Integration with Donation Box System

### Controller Integration
```cpp
// Add MQTT service to controller
controller->setMqttService(mqttService);

// Automatic logging in mode changes
void Controller::switchToNextMode() {
    String fromMode = getCurrentMode()->getName();
    // ... mode switching logic ...
    String toMode = getCurrentMode()->getName();
    
    if (mqttService && mqttService->isConnected()) {
        mqttService->modeChanged(fromMode, toMode);
        mqttService->logInfo("Mode switched: " + fromMode + " -> " + toMode);
    }
}
```

### Donation Event Reporting
```cpp
void Controller::handleDonation() {
    AbstractMode* currentMode = getCurrentMode();
    
    // Trigger mode's donation effect
    currentMode->donationTriggered();
    
    // Report via MQTT
    if (mqttService && mqttService->isConnected()) {
        mqttService->donation(currentMode->getName());
        mqttService->logInfo("Donation detected in " + currentMode->getName());
    }
}
```

## Configuration Requirements

### WiFi Credentials
```cpp
const char* WIFI_SSID = "your-network";
const char* WIFI_PASSWORD = "your-password";
```

### MQTT Broker Settings
```cpp
const char* MQTT_SERVER = "mqtt.broker.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "donation-box-01";
```

### Optional Authentication
```cpp
const char* MQTT_USER = "username";      // Set to nullptr if not needed
const char* MQTT_PASSWORD = "password";  // Set to nullptr if not needed
```

## Troubleshooting

### Connection Issues
```
ERROR: WiFi connection failed
Solutions:
1. Verify SSID and password
2. Check WiFi signal strength
3. Confirm network accessibility
4. Review Serial monitor for connection attempts
```

### MQTT Problems
```
ERROR: MQTT connection failed, rc=-2
Solutions:
1. Verify broker address and port
2. Check MQTT credentials if required
3. Confirm broker accessibility from network
4. Review MQTT client ID conflicts
```

### Message Publishing Failures
```
ERROR: Failed to publish donation
Solutions:
1. Check MQTT connection status
2. Verify topic permissions
3. Confirm message size limits
4. Review broker connection logs
```

## Monitoring and Analytics

### MQTT Message Monitoring
```bash
# Subscribe to all donation box messages
mosquitto_sub -h mqtt.broker.com -t "donation-box/+/+"

# Monitor specific events
mosquitto_sub -h mqtt.broker.com -t "donation-box/+/donations"
mosquitto_sub -h mqtt.broker.com -t "donation-box/+/logs"
```

### Data Analytics
The structured JSON messages enable easy integration with:
- **InfluxDB**: Time-series data storage
- **Grafana**: Real-time dashboards and visualization
- **Node-RED**: Flow-based message processing
- **Home Assistant**: Smart home integration

---

*MQTT connectivity for professional donation box monitoring*  
*Part of the modular donation box LED controller system*
