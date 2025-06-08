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