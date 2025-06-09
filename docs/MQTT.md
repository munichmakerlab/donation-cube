# 📡 MQTT Integration Guide

## Overview
The donation box system now includes comprehensive MQTT connectivity for remote monitoring and logging. This allows you to track donations, monitor system status, and receive real-time notifications over WiFi.

## 🚀 Quick Setup

### 1. WiFi Configuration
Copy the credentials template and configure your network:
```bash
cp include/credentials.h.example include/credentials.h
```

Edit `include/credentials.h`:
```cpp
#define WIFI_SSID         "YourWiFiNetworkName"
#define WIFI_PASSWORD     "YourWiFiPassword"
#define MQTT_SERVER       "broker.hivemq.com"    // Or your broker
```

### 2. Enable MQTT
MQTT is enabled by default. To disable:
```cpp
// In include/Config.h
#define ENABLE_MQTT         false
```

### 3. Build and Deploy
```bash
# For ESP32C3
pio run -e esp32c3 -t upload

# For ESP8266
pio run -e wemos_d1_mini -t upload
```

## 📊 MQTT Topics Structure

The system publishes to the following topic hierarchy:
```
donation-box/{clientId}/
├── donations        # Donation events
├── logs            # System log messages  
├── status          # System status updates
├── mode            # Mode change notifications
└── heartbeat       # Periodic alive signals
```

### Topic Examples
- `donation-box/donation-box-12345/donations`
- `donation-box/donation-box-12345/logs`
- `donation-box/donation-box-12345/status`
- `donation-box/donation-box-12345/mode`
- `donation-box/donation-box-12345/heartbeat`

## 📋 Message Formats

### Donation Events
```json
{
  "timestamp": "2025-06-09T14:30:00",
  "mode": "StaticMode",
  "amount": 1,
  "event": "donation"
}
```

### Log Messages
```json
{
  "timestamp": "2025-06-09T14:30:00",
  "level": "info",
  "message": "System initialized with 6 LED modes",
  "source": "donation-box"
}
```

### Mode Changes
```json
{
  "timestamp": "2025-06-09T14:30:00",
  "fromMode": "StaticMode",
  "toMode": "WaveMode",
  "event": "mode_changed"
}
```

### System Status
```json
{
  "timestamp": "2025-06-09T14:30:00",
  "status": "online",
  "uptime": 3600,
  "freeHeap": 45000,
  "wifiRSSI": -45
}
```

### Heartbeat
```json
{
  "timestamp": "2025-06-09T14:30:00",
  "status": "alive",
  "uptime": 3600
}
```

## 🔧 Configuration Options

### WiFi Settings
```cpp
// Connection timeouts
#define WIFI_TIMEOUT        10000   // 10 seconds
#define WIFI_RETRY_INTERVAL 30000   // 30 seconds

// Auto-reconnection
#define ENABLE_AUTO_RECONNECT true
```

### MQTT Settings
```cpp
// Broker configuration
#define MQTT_PORT           1883
#define MQTT_KEEPALIVE      60      // seconds
#define MQTT_QOS            0       // Quality of Service
#define MQTT_RETAIN         false

// Connection settings
#define MQTT_TIMEOUT        5000    // 5 seconds
#define MQTT_CLEAN_SESSION  true
```

### Feature Toggles
```cpp
// Enable/disable features
#define ENABLE_MQTT         true
#define ENABLE_HEARTBEAT    true
#define ENABLE_SERIAL_DEBUG true
```

## 🌐 MQTT Broker Options

### Public Brokers (Testing)
- `broker.hivemq.com` (Default)
- `test.mosquitto.org`
- `mqtt.eclipse.org`

### Local Brokers (Production)
- Mosquitto on Raspberry Pi
- Home Assistant MQTT broker
- Local Docker instance

### Cloud Brokers
- AWS IoT Core
- Google Cloud IoT
- Azure IoT Hub

## 📱 Monitoring Tools

### Command Line (mosquitto-clients)
```bash
# Subscribe to all messages
mosquitto_sub -h broker.hivemq.com -t "donation-box/+/+"

# Subscribe to donations only
mosquitto_sub -h broker.hivemq.com -t "donation-box/+/donations"

# Subscribe to logs
mosquitto_sub -h broker.hivemq.com -t "donation-box/+/logs"
```

### GUI Tools
- **MQTT Explorer** (Cross-platform GUI)
- **MQTTBox** (Browser-based)
- **MQTT.fx** (Java-based)

### Mobile Apps
- **MyMQTT** (Android)
- **MQTTool** (iOS)

## 🔍 Troubleshooting

### WiFi Connection Issues
```cpp
// Check serial output for:
[MQTT] Connecting to WiFi: YourNetwork
[MQTT] WiFi connected! IP: 192.168.1.100
```

### MQTT Connection Issues
```cpp
// Common error messages:
[MQTT] MQTT connection failed, rc=-2  // Network unreachable
[MQTT] MQTT connection failed, rc=4   // Login failed
[MQTT] MQTT connection failed, rc=5   // Not authorized
```

### Memory Issues
- Monitor heap usage in heartbeat messages
- Reduce heartbeat frequency if needed
- Disable MQTT if memory is critical

### Network Debugging
```cpp
// Enable verbose logging
#define ENABLE_SERIAL_DEBUG true

// Check WiFi signal strength
Serial.print("WiFi RSSI: ");
Serial.println(WiFi.RSSI());
```

## 🔒 Security Considerations

### Production Deployment
1. Use dedicated MQTT broker (not public)
2. Enable MQTT authentication
3. Use TLS/SSL encryption
4. Implement access control
5. Keep credentials secure

### Example Secure Config
```cpp
#define MQTT_SERVER       "your-secure-broker.com"
#define MQTT_PORT         8883  // TLS port
#define MQTT_USER         "donation_box_user"
#define MQTT_PASSWORD     "secure_password_123"
```

## 📈 Integration Examples

### Home Assistant
```yaml
# configuration.yaml
mqtt:
  sensor:
    - name: "Donation Box Status"
      state_topic: "donation-box/donation-box-12345/status"
      value_template: "{{ value_json.status }}"
    
    - name: "Donation Count"
      state_topic: "donation-box/donation-box-12345/donations"
      value_template: "{{ value_json.amount }}"
```

### Node-RED Flow
```json
[
  {
    "id": "mqtt-in",
    "type": "mqtt in",
    "topic": "donation-box/+/donations",
    "broker": "mqtt-broker"
  },
  {
    "id": "process-donation",
    "type": "function",
    "func": "msg.payload = JSON.parse(msg.payload);\nreturn msg;"
  }
]
```

### Python Script
```python
import paho.mqtt.client as mqtt
import json

def on_message(client, userdata, message):
    topic = message.topic
    payload = json.loads(message.payload.decode())
    
    if 'donations' in topic:
        print(f"Donation received: {payload['mode']} - {payload['amount']}")

client = mqtt.Client()
client.on_message = on_message
client.connect("broker.hivemq.com", 1883)
client.subscribe("donation-box/+/donations")
client.loop_forever()
```

## 🎯 Advanced Features

### Custom Topics
```cpp
// In your code, add custom publishing:
if (mqttService && mqttService->isConnected()) {
    String customTopic = "donation-box/custom/sensor-data";
    String payload = "{\"temperature\":23.5,\"humidity\":65}";
    mqttService->publish(customTopic, payload);
}
```

### Message Retention
```cpp
// Enable message retention for status
#define MQTT_RETAIN_STATUS  true
```

### Quality of Service
```cpp
// For critical messages, use QoS 1
#define MQTT_QOS_CRITICAL   1
```

## 📚 API Reference

See `lib/MqttService/README.md` for complete API documentation.

## 🤝 Contributing

When adding new MQTT features:
1. Follow the existing message format standards
2. Add proper error handling and logging
3. Test on both ESP32C3 and ESP8266
4. Update this documentation
5. Add example integration code
