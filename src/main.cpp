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
| - Home Assistant integration via MQTT                                        |
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
//#include <SoftwareSerial.h>
//#include <DFRobotDFPlayerMini.h>

// ============================================================================
//                              PIN DEFINITIONS
// ============================================================================
#define SENSOR_PIN  4 //D2 // TCRT5000 donation sensor (HIGH = donation detected)
#define DATA_PIN    3 //D3 // WS2812B LED strip data line
//#define DFPLAYER_RX D5 // DFPlayer Mini RX (ESP8266 TX -> DFPlayer RX)
//#define DFPLAYER_TX D6 // DFPlayer Mini TX (ESP8266 RX <- DFPlayer TX)

// ============================================================================
//                              LED CONFIGURATION
// ============================================================================
#define NUM_LEDS            6       // Number of LEDs in WS2812B strip
#define LED_TYPE            WS2812B // FastLED type for WS2812B LEDs
#define MAX_BRIGHTNESS      255     // Maximum LED brightness (0-255)
#define MIN_BRIGHTNESS      0       // Minimum brightness for breathing effect
#define BREATH_SPEED_NORMAL 80      // Normal breathing speed
#define BREATH_SPEED_FAST   10      // Fast breathing speed
#define BRIGHTNESS_STEP     15      // Step size for brightness change
#define DELAY               50      // Delay between brightness changes
#define EFFECT_DURATION     200     // Duration of the donation effect

// ============================================================================
//                            FUNCTION DEFINITIONS
// ============================================================================
void playSound();        // Function to play sound on DFPlayer Mini
void donationCallback(); // Callback function for donation detection

// ============================================================================
//                           HARDWARE INITIALIZATION
// ============================================================================
//SoftwareSerial      dfSerial(DFPLAYER_RX, DFPLAYER_TX); // SoftwareSerial for DFPlayer
//DFRobotDFPlayerMini dfPlayer;                           // DFPlayer Mini object
CRGB                leds[NUM_LEDS];                     // LED array for FastLED

// ============================================================================
//                            GLOBAL VARIABLES
// ============================================================================
uint8_t  current_brightness = MIN_BRIGHTNESS;      // Current brightness
uint8_t  new_brightness     = MIN_BRIGHTNESS;      // New brightness for animation

uint64_t counter            = 0;                   // Counter for animation timing
uint16_t speed              = BREATH_SPEED_NORMAL; // Speed of the breathing effect
uint8_t  direction          = 1;                   // Direction of brightness change (1 = increase, -1 = decrease)
uint8_t  sensor_state       = HIGH;                // Current state of the sensor button (HIGH = no donation, LOW = donation detected)
uint8_t  sensor_threshold   = 100;                 // Threshold for sensor detection (adjust as needed)

uint64_t effect_counter     = 0;                   // Counter for the donation effect

// ============================================================================
//                              SETUP FUNCTION
// ============================================================================
void setup() {
  Serial.begin(115200);

  // Pin configuration
  pinMode(SENSOR_PIN, INPUT_PULLUP);

  // Initialize LED strip
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(current_brightness); // Set initial brightness
  FastLED.clear();                           // Clear the LED strip

  // Set White color for breathing effect
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
  }

  FastLED.show();

  Serial.println("[INFO] Setup complete. Waiting for donations…");
}

// ============================================================================
//                                MAIN LOOP
// ============================================================================
void loop() {
  // Increment the counter
  counter++;

  // Activate the breathing effect
  if (digitalRead(SENSOR_PIN) != sensor_state && sensor_state == HIGH) {
    Serial.println("[INFO] Donation detected! Starting animation…");

    effect_counter = counter;  // Start the donation effect
    speed = BREATH_SPEED_FAST; // Speed up the animation
  
    donationCallback();        // Call the donation callback function
  }

  // Disable the donation effect after a certain duration
  if (effect_counter > 0 && counter - effect_counter >= EFFECT_DURATION) {
    Serial.println("[INFO] Donation effect ended.");

    effect_counter = 0;
    speed = BREATH_SPEED_NORMAL; // Reset speed to normal after effect
  }

  sensor_state = digitalRead(SENSOR_PIN);

  if (direction) {
    if (new_brightness + BRIGHTNESS_STEP >= MAX_BRIGHTNESS) {
      new_brightness = MAX_BRIGHTNESS;
      direction = 0;
    } else {
      new_brightness += BRIGHTNESS_STEP;
    }
  } else {
    if (new_brightness - BRIGHTNESS_STEP <= MIN_BRIGHTNESS) {
      new_brightness = MIN_BRIGHTNESS;
      direction = 1;
    } else {
      new_brightness -= BRIGHTNESS_STEP;
    }
  }

  // Update LED brightness
  if (new_brightness != current_brightness) {
    current_brightness = new_brightness;
    FastLED.setBrightness(current_brightness);
    FastLED.show();
  }

  // Delay to control animation speed
  delay(speed);
}

// ============================================================================
//                                   FUNCTIONS
// ============================================================================
void playSound() {
  // Placeholder for sound playback logic
  // This function should interface with the DFPlayer Mini to play a sound
  Serial.println("[INFO] Playing sound for donation effect.");
}

void donationCallback() {
  // Placeholder for donation callback logic
  // This function can be used to handle additional actions when a donation is detected
  Serial.println("[INFO] Donation callback triggered.");
  playSound(); // Play sound effect
}
