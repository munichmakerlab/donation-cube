/*
+==============================================================================+
|                          DONATION BOX LED CONTROLLER                         |
|                              Version 1.0 – 2025                              |
+==============================================================================+
|                                                                              |
| DESCRIPTION:                                                                 |
| Automatic LED animation and sound effects for a donation box.                |
| When a donation is detected (object passes through the sensor), a            |
| spectacular LED animation is triggered and a sound effect is played.         |
|                                                                              |
| FEATURES:                                                                    |
| - Continuous LED breathing effect (white) in standby                         |
| - Accelerated pulse animation when donation is detected (5 seconds)          |
| - Sound effects via DFPlayer Mini                                            |
| - 2-second debounce to prevent sensor flicker                                |
|                                                                              |
| HARDWARE:                                                                    |
| - ESP8266 (Wemos D1 Mini)                                                    |
| - WS2812B LED strip (6 LEDs)                                                 |
| - TCRT5000 reflective sensor                                                 |
| - DFPlayer Mini MP3 module                                                   |
| - MicroSD card with MP3 files                                                |
+==============================================================================+

+------------------------------------------------------------------------------+
|                              PIN CONFIGURATION                               |
+------------------------------------------------------------------------------+
| PIN   | FUNCTION     | DESCRIPTION                                           |
+-------+--------------+-------------------------------------------------------+
| D2    | SENSOR_PIN   | Input for TCRT5000 donation sensor                    |
|       |              | HIGH = Donation detected                              |
|       |              | INPUT_PULLUP enabled                                  |
+-------+--------------+-------------------------------------------------------+
| D3    | DATA_PIN     | Data line to WS2812B LED strip                        |
|       |              | Output for FastLED library                            |
+-------+--------------+-------------------------------------------------------+
| D5    | DFPLAYER_RX  | Receive from DFPlayer Mini (TX → RX)                  |
|       |              | SoftwareSerial connection                             |
+-------+--------------+-------------------------------------------------------+
| D6    | DFPLAYER_TX  | Send to DFPlayer Mini (RX ← TX)                       |
|       |              | SoftwareSerial connection                             |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                            DFPLAYER WIRING                                   |
+------------------------------------------------------------------------------+
| DFPLAYER  | ESP8266   | DESCRIPTION                                          |
+-----------+-----------+------------------------------------------------------+
| VCC       | 5V/3.3V   | Power supply (depending on module)                   |
| GND       | GND       | Ground                                               |
| TX        | D5        | DFPlayer output → ESP8266 input                      |
| RX        | D6        | DFPlayer input ← ESP8266 output                      |
| SPK+      | Speaker+  | Speaker positive                                     |
| SPK-      | Speaker-  | Speaker negative                                     |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                              LED CONFIGURATION                               |
+------------------------------------------------------------------------------+
| PARAMETER     | VALUE     | DESCRIPTION                                      |
+---------------+-----------+--------------------------------------------------+
| NUM_LEDS      | 6         | Number of WS2812B LEDs in the strip              |
| LED_TYPE      | WS2812B   | FastLED type for WS2812B LEDs                    |
| MAX_BRIGHTNESS| 255       | Maximum brightness (0–255)                       |
| MIN_BRIGHTNESS| 20        | Minimum brightness for breathing effect          |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                              MICROSD SETUP                                   |
+------------------------------------------------------------------------------+
| Format: FAT32                                                                |
| Files in root directory:                                                     |
| - 001.mp3  → Main sound for donation detection                               |
| - 002.mp3  → Optional: additional sounds                                     |
| - 003.mp3  → Optional: additional sounds                                     |
|                                                                              |
| Supported formats: MP3, WAV                                                  |
| Bitrate: 8–320 kbps                                                          |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                             ANIMATION SEQUENCE                               |
+------------------------------------------------------------------------------+
| MODE      | DURATION     | EFFECT                                            |
+-----------+--------------+---------------------------------------------------+
| Standby   | Continuous   | Slow breathing (white) - brightness between       |
|           |              | 20-255 with 2000ms cycle                          |
+-----------+--------------+---------------------------------------------------+
| Animation | 5 seconds    | Accelerated breathing - speed increases           |
|           |              | parabolically (max. 200ms cycle) and returns      |
|           |              | to normal speed                                   |
+-----------+--------------+---------------------------------------------------+
| Debounce: 2 seconds pause between animations                                 |
+------------------------------------------------------------------------------+
*/

#include <Arduino.h>
#include <FastLED.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// ============================================================================
//                              PIN DEFINITIONS
// ============================================================================
#define SENSOR_PIN D2        // TCRT5000 donation sensor (HIGH = donation detected)
#define DATA_PIN D3          // WS2812B LED strip data line
#define DFPLAYER_RX D5       // DFPlayer Mini RX (ESP8266 TX -> DFPlayer RX)
#define DFPLAYER_TX D6       // DFPlayer Mini TX (ESP8266 RX <- DFPlayer TX)

// ============================================================================
//                              LED CONFIGURATION
// ============================================================================
#define NUM_LEDS 6                // Number of LEDs in WS2812B strip
#define LED_TYPE WS2812B          // FastLED type for WS2812B LEDs
#define MAX_BRIGHTNESS 255        // Maximum LED brightness (0-255)
#define MIN_BRIGHTNESS 20         // Minimum brightness for breathing effect
#define BREATH_SPEED_NORMAL 2000  // Normal breathing speed in ms
#define BREATH_SPEED_FAST 200     // Fast breathing speed in ms
#define BRIGHTNESS_STEP 15        // Step size for brightness change

// ============================================================================
//                           HARDWARE INITIALIZATION
// ============================================================================
SoftwareSerial dfSerial(DFPLAYER_RX, DFPLAYER_TX);  // SoftwareSerial for DFPlayer
DFRobotDFPlayerMini dfPlayer;                        // DFPlayer Mini object
CRGB leds[NUM_LEDS];                                 // LED array for FastLED

// ============================================================================
//                            GLOBAL VARIABLES
// ============================================================================
uint8_t current_brightness = MIN_BRIGHTNESS;   // Current brightness
uint8_t brightness_step = BRIGHTNESS_STEP;     // Brightness step size
bool brightness_direction = true;              // true = increasing, false = decreasing
uint16_t breath_counter = 0;                   // Counter for breathing timing
bool animation_active = false;                 // Flag: animation is running
uint16_t animation_counter = 0;                // Counter for animation duration
uint16_t debounce_counter = 0;                 // Counter for debounce
bool sensor_was_low = true;                    // Status for edge detection

// ============================================================================
//                              SETUP FUNCTION
// ============================================================================
void setup() {
  Serial.begin(115200);

  // Pin configuration
  pinMode(SENSOR_PIN, INPUT_PULLUP);          // Sensor with pull-up resistor

  // Initialize LED strip
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setCorrection(CRGB(200, 40, 80)); // Color correction to white

  // Initialize DFPlayer Mini
  dfSerial.begin(9600);
  Serial.println();
  Serial.println("+=============================================+");
  Serial.println("|        DONATION BOX LED CONTROLLER         |");
  Serial.println("+=============================================+");
  Serial.println("Initializing DFPlayer Mini...");

  if (!dfPlayer.begin(dfSerial)) {
    Serial.println("ERROR: DFPlayer Mini not connected!");
    Serial.println("Please check:");
    Serial.println("1. Wiring (RX->D5, TX->D6, VCC->5V, GND->GND)");
    Serial.println("2. Inserted MicroSD card");
    Serial.println("3. MP3 files on the SD card");
  } else {
    Serial.println("DFPlayer Mini initialized successfully!");
    dfPlayer.volume(25);                      // Set volume to 25/30
    delay(100);
  }

  delay(1000);
  Serial.println("Donation Box LED Controller ready!");
  Serial.println("Waiting for donations...");
}

// ============================================================================
//                                MAIN LOOP
// ============================================================================
void loop() {
  // Reduce debounce counter if active
  if (debounce_counter > 0) {
    debounce_counter--;
  }

  // Calculate breathing speed based on animation status
  uint16_t breath_delay;

  if (animation_active) {
    // During animation: speed based on progress
    float progress = (float)animation_counter / 500.0; // 500 = 5 seconds at 10ms delay

    if (progress >= 1.0) {
      // End animation
      animation_active = false;
      animation_counter = 0;
      breath_delay = 8; // Normal: 2000ms / 256 steps ≈ 8ms per step
      brightness_step = BRIGHTNESS_STEP; // Return to normal step size
      Serial.println("Animation ended - returning to normal breathing");
    } else {
      // Parabolic speed profile: slow -> fast -> slow
      float speed_factor;
      if (progress < 0.5) {
        speed_factor = progress * 2.0;
      } else {
        speed_factor = 2.0 - (progress * 2.0);
      }

      breath_delay = 8 - (uint16_t)(speed_factor * 7.0);
      if (breath_delay < 1) breath_delay = 1;

      animation_counter++;
    }
  } else {
    breath_delay = 8;
  }

  // Update breathing effect every X cycles
  if (breath_counter >= breath_delay) {
    breath_counter = 0;

    if (brightness_direction) {
      if (current_brightness + BRIGHTNESS_STEP >= MAX_BRIGHTNESS) {
        brightness_direction = false;
      } else {
        current_brightness += brightness_step;
      }
    } else {
      if (current_brightness - BRIGHTNESS_STEP <= MIN_BRIGHTNESS) {
        brightness_direction = true;
      } else {
        current_brightness -= brightness_step;
      }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(current_brightness, current_brightness, current_brightness);
    }
    FastLED.show();
    delay(10);

    Serial.print("Current brightness: ");
    Serial.println(current_brightness);
  } else {
    breath_counter++;
  }

  // Check donation sensor with edge detection and debounce
  bool sensor_current = digitalRead(SENSOR_PIN);

  if (sensor_current == HIGH && sensor_was_low && !animation_active && debounce_counter == 0) {
    Serial.println("*** DONATION DETECTED! Starting animation and sound! ***");

    dfPlayer.play(1); // Play 001.mp3 from SD card

    animation_active = true;
    animation_counter = 0;
    debounce_counter = 200; // 200 * 10ms = 2 seconds debounce
  }

  sensor_was_low = (sensor_current == LOW);

  delay(10); // 10ms delay for stable timing
}
