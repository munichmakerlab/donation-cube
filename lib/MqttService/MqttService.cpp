#include "MqttService.hpp"

MqttService::MqttService(const char* ssid, const char* password, 
                         const char* server, int port,
                         const char* clientId, const char* user, const char* pass)
#if ENABLE_WIFI
    : wifiSSID(ssid), wifiPassword(password),
      mqttServer(server), mqttPort(port), mqttClientId(clientId),
      mqttUser(user), mqttPassword(pass),
      mqttClient(wifiClient),
      wifiConnected(false), mqttConnected(false),
      lastReconnectAttempt(0), lastHeartbeat(0) {
    
    // Set default base topic
    baseTopic = String("donation-box/") + String(clientId);
    donationTopic = baseTopic + "/donations";
    logTopic = baseTopic + "/logs";
    statusTopic = baseTopic + "/status";
    modeTopic = baseTopic + "/mode";
    
    // Configure MQTT client
    mqttClient.setServer(mqttServer, mqttPort);
#else
    {
    // Dummy mode - WiFi/MQTT disabled
    Serial.println("[MQTT] WiFi/MQTT disabled - running in standalone mode");
    dummyConnected = false;
#endif
}

void MqttService::setup() {
#if ENABLE_WIFI
    Serial.println("[MQTT] MqttService setup started");
    
    // Initialize WiFi
    connectWiFi();
    
    if (wifiConnected) {
        // Initialize MQTT connection
        connectMQTT();
        
        if (mqttConnected) {
            // Send startup message
            systemStatus("online");
            logInfo("Donation box system started");
        }
    }
    
    Serial.println("[MQTT] MqttService setup complete");
#else
    Serial.println("[MQTT] Standalone mode - no network features enabled");
    dummyConnected = true; // Simulate successful "connection"
#endif
}

void MqttService::loop() {
#if ENABLE_WIFI
    unsigned long currentTime = millis();
    
    // Maintain WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnected = false;
        if (currentTime - lastReconnectAttempt >= RECONNECT_INTERVAL) {
            connectWiFi();
            lastReconnectAttempt = currentTime;
        }
    } else {
        wifiConnected = true;
    }
    
    // Maintain MQTT connection
    if (wifiConnected) {
        if (!mqttClient.connected()) {
            mqttConnected = false;
            if (currentTime - lastReconnectAttempt >= RECONNECT_INTERVAL) {
                if (reconnect()) {
                    lastReconnectAttempt = 0;
                } else {
                    lastReconnectAttempt = currentTime;
                }
            }
        } else {
            mqttConnected = true;
            mqttClient.loop(); // Process MQTT messages
        }
        
        // Send heartbeat
        if (mqttConnected && currentTime - lastHeartbeat >= HEARTBEAT_INTERVAL) {
            publishHeartbeat();
            lastHeartbeat = currentTime;
        }
    }
#else
    // Dummy mode - no actual network operations
    // Just maintain the "connected" state for compatibility
#endif
}

void MqttService::donation(const String& mode, int amount) {
#if ENABLE_WIFI
    if (!mqttConnected) {
        Serial.println("[MQTT] Cannot send donation - not connected");
        return;
    }
    
    String payload = "{";
    payload += "\"timestamp\":\"" + formatTimestamp() + "\",";
    payload += "\"mode\":\"" + mode + "\",";
    payload += "\"amount\":" + String(amount) + ",";
    payload += "\"event\":\"donation\"";
    payload += "}";
    
    if (mqttClient.publish(donationTopic.c_str(), payload.c_str())) {
        Serial.println("[MQTT] Donation published: " + mode);
    } else {
        Serial.println("[MQTT] Failed to publish donation");
    }
#else
    Serial.println("[MQTT] Standalone mode - donation logged locally: " + mode);
#endif
}

void MqttService::logInfo(const String& message) {
#if ENABLE_WIFI
    if (!mqttConnected) {
        return; // Fail silently for logs
    }
    
    String payload = "{";
    payload += "\"timestamp\":\"" + formatTimestamp() + "\",";
    payload += "\"level\":\"INFO\",";
    payload += "\"message\":\"" + message + "\"";
    payload += "}";
    
    mqttClient.publish(logTopic.c_str(), payload.c_str());
#else
    // In standalone mode, just log to serial
    Serial.println("[INFO] " + message);
#endif
}

void MqttService::logWarning(const String& message) {
#if ENABLE_WIFI
    if (!mqttConnected) {
        return;
    }
    
    String payload = "{";
    payload += "\"timestamp\":\"" + formatTimestamp() + "\",";
    payload += "\"level\":\"WARNING\",";
    payload += "\"message\":\"" + message + "\"";
    payload += "}";
    
    mqttClient.publish(logTopic.c_str(), payload.c_str());
    Serial.println("[MQTT] Warning logged: " + message);
#else
    Serial.println("[WARNING] " + message);
#endif
}

void MqttService::logError(const String& message) {
#if ENABLE_WIFI
    if (!mqttConnected) {
        return;
    }
    
    String payload = "{";
    payload += "\"timestamp\":\"" + formatTimestamp() + "\",";
    payload += "\"level\":\"ERROR\",";
    payload += "\"message\":\"" + message + "\"";
    payload += "}";
    
    mqttClient.publish(logTopic.c_str(), payload.c_str());
    Serial.println("[MQTT] Error logged: " + message);
#else
    Serial.println("[ERROR] " + message);
#endif
}

void MqttService::modeChanged(const String& fromMode, const String& toMode) {
#if ENABLE_WIFI
    if (!mqttConnected) {
        Serial.println("[MQTT] Cannot send mode change - not connected");
        return;
    }
    
    String payload = "{";
    payload += "\"timestamp\":\"" + formatTimestamp() + "\",";
    payload += "\"from_mode\":\"" + fromMode + "\",";
    payload += "\"to_mode\":\"" + toMode + "\",";
    payload += "\"event\":\"mode_change\"";
    payload += "}";
    
    if (mqttClient.publish(modeTopic.c_str(), payload.c_str())) {
        Serial.println("[MQTT] Mode change published: " + fromMode + " -> " + toMode);
    } else {
        Serial.println("[MQTT] Failed to publish mode change");
    }
#else
    Serial.println("[MQTT] Standalone mode - mode changed: " + fromMode + " -> " + toMode);
#endif
}

void MqttService::systemStatus(const String& status) {
#if ENABLE_WIFI
    if (!mqttConnected) {
        return;
    }
    
    String payload = "{";
    payload += "\"timestamp\":\"" + formatTimestamp() + "\",";
    payload += "\"status\":\"" + status + "\",";
    payload += "\"wifi_connected\":" + String(wifiConnected ? "true" : "false") + ",";
    payload += "\"mqtt_connected\":" + String(mqttConnected ? "true" : "false") + ",";
    payload += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
    payload += "\"uptime\":" + String(millis());
    payload += "}";
    
    mqttClient.publish(statusTopic.c_str(), payload.c_str());
#else
    Serial.println("[MQTT] Standalone mode - system status: " + status);
#endif
}

bool MqttService::isConnected() const {
#if ENABLE_WIFI
    return mqttConnected;
#else
    return dummyConnected; // Always "connected" in standalone mode
#endif
}

bool MqttService::isWiFiConnected() const {
#if ENABLE_WIFI
    return wifiConnected;
#else
    return false; // No WiFi in standalone mode
#endif
}

String MqttService::getConnectionStatus() const {
#if ENABLE_WIFI
    if (!wifiConnected) {
        return "WiFi disconnected";
    } else if (!mqttConnected) {
        return "WiFi connected, MQTT disconnected";
    } else {
        return "Fully connected";
    }
#else
    return "Standalone mode (no network)";
#endif
}

void MqttService::setBaseTopic(const String& topic) {
#if ENABLE_WIFI
    baseTopic = topic;
    donationTopic = baseTopic + "/donations";
    logTopic = baseTopic + "/logs";
    statusTopic = baseTopic + "/status";
    modeTopic = baseTopic + "/mode";
#else
    // Do nothing in standalone mode
#endif
}

#if ENABLE_WIFI
// Private methods (only available when WiFi is enabled)

void MqttService::connectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        return;
    }
    
    Serial.print("[MQTT] Connecting to WiFi: ");
    Serial.println(wifiSSID);
    
    WiFi.begin(wifiSSID, wifiPassword);
    
    // Wait up to 10 seconds for connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println();
        Serial.print("[MQTT] WiFi connected! IP: ");
        Serial.println(WiFi.localIP());
    } else {
        wifiConnected = false;
        Serial.println();
        Serial.println("[MQTT] WiFi connection failed");
    }
}

void MqttService::connectMQTT() {
    if (!wifiConnected) {
        return;
    }
    
    Serial.print("[MQTT] Connecting to MQTT broker: ");
    Serial.print(mqttServer);
    Serial.print(":");
    Serial.println(mqttPort);
    
    bool connected;
    if (mqttUser && mqttPassword) {
        connected = mqttClient.connect(mqttClientId, mqttUser, mqttPassword);
    } else {
        connected = mqttClient.connect(mqttClientId);
    }
    
    if (connected) {
        mqttConnected = true;
        Serial.println("[MQTT] MQTT connected!");
    } else {
        mqttConnected = false;
        Serial.print("[MQTT] MQTT connection failed, rc=");
        Serial.println(mqttClient.state());
    }
}

bool MqttService::reconnect() {
    if (!wifiConnected) {
        return false;
    }
    
    Serial.print("[MQTT] Attempting MQTT reconnection...");
    
    bool connected;
    if (mqttUser && mqttPassword) {
        connected = mqttClient.connect(mqttClientId, mqttUser, mqttPassword);
    } else {
        connected = mqttClient.connect(mqttClientId);
    }
    
    if (connected) {
        mqttConnected = true;
        Serial.println(" connected!");
        systemStatus("reconnected");
        return true;
    } else {
        mqttConnected = false;
        Serial.print(" failed, rc=");
        Serial.println(mqttClient.state());
        return false;
    }
}

void MqttService::publishHeartbeat() {
    String payload = "{";
    payload += "\"timestamp\":\"" + formatTimestamp() + "\",";
    payload += "\"event\":\"heartbeat\",";
    payload += "\"uptime\":" + String(millis()) + ",";
    payload += "\"free_heap\":" + String(ESP.getFreeHeap());
    payload += "}";
    
    mqttClient.publish((baseTopic + "/heartbeat").c_str(), payload.c_str());
}

String MqttService::formatTimestamp() {
    // Simple timestamp using millis() - in production you'd use NTP time
    unsigned long now = millis();
    unsigned long seconds = now / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    String timestamp = String(hours % 24) + ":";
    timestamp += String((minutes % 60) < 10 ? "0" : "") + String(minutes % 60) + ":";
    timestamp += String((seconds % 60) < 10 ? "0" : "") + String(seconds % 60);
    
    return timestamp;
}
#endif
