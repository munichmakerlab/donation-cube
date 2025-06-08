#include <Arduino.h>

// Include modular services and controller
#include "Controller.hpp"
#include "LightService.hpp"
#include "SpeakerService.hpp"
#include "SensorService.hpp"

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
  Serial.begin(115200);
  Serial.println("[INFO] Starting modular donation box system...");

  // Initialize services
  lightService = new LightService();
  speakerService = new SpeakerService();
  sensorService = new SensorService(SENSOR_PIN);

  // Setup services
  lightService->setup();
  speakerService->setup();
  sensorService->setup();

  // Create controller
  controller = new Controller(sensorService);

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

  Serial.println("[INFO] Setup complete. Donation box ready!");
}

// ============================================================================
//                                MAIN LOOP
// ============================================================================
void loop() {
  // Update sensor service
  sensorService->loop();
  
  // Run controller logic
  controller->loop();
}
