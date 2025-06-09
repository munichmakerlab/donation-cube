// ============================================================================
//                              PIN DEFINITIONS
// ============================================================================

// Hardware-specific pin definitions
#ifdef ESP8266
  // ESP8266 (Wemos D1 Mini) pin configuration
  #define SENSOR_PIN  D2  // GPIO4 - TCRT5000 donation sensor
  #define DATA_PIN    D3  // GPIO0 - WS2812B LED strip data line
#elif defined(ESP32)
  // ESP32-C3 (Seeed XIAO) pin configuration  
  #define SENSOR_PIN  4   // GPIO4 - TCRT5000 donation sensor
  #define DATA_PIN    3   // GPIO3 - WS2812B LED strip data line
#else
  // Default fallback configuration
  #define SENSOR_PIN  4
  #define DATA_PIN    3
#endif

// Legacy DFPlayer pins (currently unused)
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
//                             WIFI CONFIGURATION
// ============================================================================
// Optional: Include user-specific credentials
// Copy credentials.h.example to credentials.h and fill in your details
// Or run the setup script: python3 setup.py
#ifdef __has_include
  #if __has_include("credentials.h")
    #include "credentials.h"
  #endif
#endif

// Connection timeouts
#define WIFI_TIMEOUT        10000   // WiFi connection timeout in milliseconds
#define WIFI_RETRY_INTERVAL 30000   // WiFi retry interval in milliseconds

// ============================================================================
//                             MQTT CONFIGURATION
// ============================================================================

// MQTT Client configuration
#define MQTT_CLIENT_ID      "donation-box"      // Base client ID (will be made unique)
#define MQTT_BASE_TOPIC     "donation-box"      // Base topic for all messages
#define MQTT_QOS            0                   // Quality of Service level
#define MQTT_RETAIN         false               // Retain messages flag

// MQTT Connection settings
#define MQTT_KEEPALIVE      60                  // Keep-alive interval in seconds
#define MQTT_CLEAN_SESSION  true                // Clean session flag
#define MQTT_TIMEOUT        5000                // MQTT connection timeout in milliseconds

// ============================================================================
//                          FEATURE TOGGLES
// ============================================================================
// Network features (can be overridden in credentials.h)
#ifndef ENABLE_WIFI
#define ENABLE_WIFI         1                   // Enable WiFi connectivity (0 = standalone mode)
#endif

#ifndef ENABLE_MQTT
#define ENABLE_MQTT         ENABLE_WIFI         // MQTT depends on WiFi
#endif

// Other optional features
#define ENABLE_SERIAL_DEBUG 1                   // Enable Serial debug output
#define ENABLE_HEARTBEAT    ENABLE_MQTT         // Enable MQTT heartbeat messages
#define ENABLE_AUTO_RECONNECT ENABLE_WIFI       // Enable automatic WiFi/MQTT reconnection