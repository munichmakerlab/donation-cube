#pragma once

#include <Arduino.h>
#include "Config.h"

#if ENABLE_WIFI
#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif
#include <PubSubClient.h>
#endif

class MqttService {
private:
#if ENABLE_WIFI
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    
    // Configuration
    const char* wifiSSID;
    const char* wifiPassword;
    const char* mqttServer;
    int mqttPort;
    const char* mqttClientId;
    const char* mqttUser;
    const char* mqttPassword;
    
    // Topics
    String baseTopic;
    String donationTopic;
    String logTopic;
    String statusTopic;
    String modeTopic;
    
    // State management
    bool wifiConnected;
    bool mqttConnected;
    unsigned long lastReconnectAttempt;
    unsigned long lastHeartbeat;
    static const unsigned long RECONNECT_INTERVAL = 5000;
    static const unsigned long HEARTBEAT_INTERVAL = 30000;
    
    // Internal methods
    void connectWiFi();
    void connectMQTT();
    bool reconnect();
    void publishHeartbeat();
    String formatTimestamp();
#else
    // Dummy mode - no actual network functionality
    bool dummyConnected = false;
#endif
    
public:
    MqttService(const char* ssid, const char* password, 
                const char* server, int port = 1883,
                const char* clientId = "donation-box",
                const char* user = nullptr, const char* pass = nullptr);
    
    // Core service methods
    void setup();
    void loop();
    
    // Publishing methods
    void donation(const String& mode, int amount = 1);
    void logInfo(const String& message);
    void logWarning(const String& message);
    void logError(const String& message);
    void modeChanged(const String& fromMode, const String& toMode);
    void systemStatus(const String& status);
    
    // Status methods
    bool isConnected() const;
    bool isWiFiConnected() const;
    String getConnectionStatus() const;
    
    // Configuration methods
    void setBaseTopic(const String& topic);
};
