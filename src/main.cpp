#include <Arduino.h>

// Include modular services and controller
#include "Controller.hpp"
#include "LightService.hpp"
#include "SpeakerService.hpp"
#include "SensorService.hpp"
#include "MqttService.hpp"

// Include available modes
#include "StaticMode.hpp"
#include "WaveMode.hpp"
#include "BlinkMode.hpp"
#include "HalfMode.hpp"
#include "CenterMode.hpp"
#include "ChaseMode.hpp"

#include "Config.h"

// ============================================================================
//                           SERVICE INSTANCES
// ============================================================================
LightService* lightService;
SpeakerService* speakerService;
SensorService* sensorService;
MqttService* mqttService;

// ============================================================================
//                           GLOBAL STATE TRACKING
// ============================================================================
// For MQTT integration - track state changes
String lastModeName = "none";
unsigned long lastDonationTime = 0;
bool donationJustDetected = false;

// ============================================================================
//                           CONTROLLER AND MODES
// ============================================================================
Controller* controller;
StaticMode* staticMode;
WaveMode* waveMode;
BlinkMode* blinkMode;
HalfMode* halfMode;
CenterMode* centerMode;
ChaseMode* chaseMode;

// ============================================================================
//                              SETUP FUNCTION
// ============================================================================
void setup() {
  // Ensure ESP is fully booted before starting
  delay(1000);
  
#if ENABLE_SERIAL_DEBUG
  // Initialize Serial but don't wait for connection
  Serial.begin(115200);
  
  // Short delay but don't wait for Serial connection
  delay(100);
  
  Serial.println("[INFO] Starting modular donation box system...");
  
  // Platform-specific boot reason logging
#ifdef ESP32
  Serial.print("[INFO] ESP32 Boot reason: ");
  Serial.println(esp_reset_reason());
#elif defined(ESP8266)
  Serial.print("[INFO] ESP8266 Reset reason: ");
  Serial.println(ESP.getResetReason());
#else
  Serial.println("[INFO] Unknown platform");
#endif
#endif

  // Initialize services with error handling (only output if Serial available)
#if ENABLE_SERIAL_DEBUG
  Serial.println("[INFO] Initializing LightService...");
#endif
  lightService = new LightService();
  
#if ENABLE_SERIAL_DEBUG
  Serial.println("[INFO] Initializing SpeakerService...");
#endif
  speakerService = new SpeakerService();
  
#if ENABLE_SERIAL_DEBUG
  Serial.println("[INFO] Initializing SensorService...");
#endif
  sensorService = new SensorService(SENSOR_PIN);

#if ENABLE_MQTT
  // Initialize MQTT service if enabled
  String uniqueClientId = String(MQTT_CLIENT_ID) + "-" + String(random(10000, 99999));
  mqttService = new MqttService(
    WIFI_SSID, 
    WIFI_PASSWORD,
    MQTT_SERVER,
    MQTT_PORT,
    uniqueClientId.c_str(),
    MQTT_USER,
    MQTT_PASSWORD
  );
  mqttService->setBaseTopic(MQTT_BASE_TOPIC);
#endif

  // Setup services
  lightService->setup();
  speakerService->setup();
  sensorService->setup();

#if ENABLE_MQTT
  if (mqttService) {
    mqttService->setup();
  }
#endif

  // Create controller
  controller = new Controller(sensorService, speakerService);

  // Create and register modes
  staticMode = new StaticMode(lightService, speakerService);
  waveMode = new WaveMode(lightService, speakerService);
  blinkMode = new BlinkMode(lightService, speakerService);
  halfMode = new HalfMode(lightService, speakerService);
  centerMode = new CenterMode(lightService, speakerService);
  chaseMode = new ChaseMode(lightService, speakerService);
  
  controller->addMode(staticMode);
  controller->addMode(waveMode);
  controller->addMode(blinkMode);
  controller->addMode(halfMode);
  controller->addMode(centerMode);
  controller->addMode(chaseMode);

  // Setup controller (this will activate the first mode)
  controller->setup();

#if ENABLE_MQTT
  // Send system startup notification
  if (mqttService && mqttService->isConnected()) {
    mqttService->systemStatus("Donation box system started successfully");
    mqttService->logInfo("System initialized with " + String(controller->getModeCount()) + " LED modes");
    mqttService->modeChanged("none", controller->getCurrentModeName());
  }
#endif

  // Initialize state tracking
  lastModeName = controller->getCurrentModeName();

#if ENABLE_SERIAL_DEBUG
  Serial.println("[INFO] Setup complete. Donation box ready!");
#endif
}

// ============================================================================
//                                MAIN LOOP
// ============================================================================
void loop() {
  // Update sensor service
  sensorService->loop();
  
  // Update speaker service
  speakerService->loop();
  
#if ENABLE_MQTT
  // Update MQTT service if enabled
  if (mqttService) {
    mqttService->loop();
  }
#endif

  // Run controller logic
  controller->loop();
  
#if ENABLE_MQTT
  // Handle MQTT events based on controller state changes
  if (mqttService && mqttService->isConnected()) {
    
    // Check for mode changes
    String currentModeName = controller->getCurrentModeName();
    if (currentModeName != lastModeName) {
      mqttService->modeChanged(lastModeName, currentModeName);
      lastModeName = currentModeName;
    }
    
    // Send donation event if just detected
    if (donationJustDetected) {
      mqttService->donation(currentModeName, 1);
      donationJustDetected = false;
    }
  }
#endif
}
