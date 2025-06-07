/*
+==============================================================================+
|                          SPENDENBOX LED CONTROLLER                           |
|                             Version 1.0 – 2025                               |
+==============================================================================+
|                                                                              |
| BESCHREIBUNG:                                                                |
| Automatische LED-Animation und Sound-Effekte für eine Spendenbox.            |
| Bei erkannter Spende (Gegenstand fällt durch Sensor) wird eine spektaku-     |
| läre LED-Animation gestartet und ein Sound-Effekt abgespielt.                |
|                                                                              |
| FUNKTIONEN:                                                                  |
| - Kontinuierliche LED-Farbrotation im Standby                                |
| - Spektakuläre 800 ms Animation bei Spendenerkennung                         |
| - Sound-Effekte über DFPlayer Mini                                           |
| - 2 Sekunden Debounce gegen Sensor-Flackern                                  |
|                                                                              |
| HARDWARE:                                                                    |
| - ESP8266 (Wemos D1 Mini)                                                    |
| - WS2812B LED-Strip (6 LEDs)                                                 |
| - TCRT5000 Reflexions-Sensor                                                 |
| - DFPlayer Mini MP3-Modul                                                    |
| - MicroSD-Karte mit MP3-Dateien                                              |
+==============================================================================+

+------------------------------------------------------------------------------+
|                              PIN KONFIGURATION                               |
+------------------------------------------------------------------------------+
| PIN   | FUNKTION     | BESCHREIBUNG                                          |
+-------+--------------+-------------------------------------------------------+
| D2    | SENSOR_PIN   | Eingang für TCRT5000 Spenden-Sensor                   |
|       |              | HIGH = Spende erkannt                                 |
|       |              | INPUT_PULLUP aktiviert                                |
+-------+--------------+-------------------------------------------------------+
| D3    | DATA_PIN     | Datenleitung zu WS2812B LED-Strip                     |
|       |              | Ausgabe für FastLED Library                           |
+-------+--------------+-------------------------------------------------------+
| D5    | DFPLAYER_RX  | Empfang vom DFPlayer Mini (TX → RX)                   |
|       |              | SoftwareSerial Verbindung                             |
+-------+--------------+-------------------------------------------------------+
| D6    | DFPLAYER_TX  | Senden zum DFPlayer Mini (RX ← TX)                    |
|       |              | SoftwareSerial Verbindung                             |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                            VERKABELUNG DFPLAYER                              |
+------------------------------------------------------------------------------+
| DFPLAYER  | ESP8266   | BESCHREIBUNG                                         |
+-----------+-----------+------------------------------------------------------+
| VCC       | 5V/3.3V   | Stromversorgung (je nach Modul)                      |
| GND       | GND       | Masse                                                |
| TX        | D5        | DFPlayer Ausgang → ESP8266 Eingang                   |
| RX        | D6        | DFPlayer Eingang ← ESP8266 Ausgang                   |
| SPK+      | Speaker+  | Lautsprecher positiv                                 |
| SPK-      | Speaker-  | Lautsprecher negativ                                 |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                              LED KONFIGURATION                               |
+------------------------------------------------------------------------------+
| PARAMETER     | WERT     | BESCHREIBUNG                                      |
+---------------+----------+---------------------------------------------------+
| NUM_LEDS      | 6        | Anzahl der WS2812B LEDs im Strip                  |
| LED_TYPE      | WS2812B  | FastLED Typ für WS2812B LEDs                      |
| BRIGHTNESS    | 255      | Maximale Helligkeit (0–255)                       |
| SATURATION    | 255      | Maximale Farbsättigung (0–255)                    |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                               MICROSD SETUP                                  |
+------------------------------------------------------------------------------+
| Format: FAT32                                                                |
| Dateien im Root-Verzeichnis:                                                 |
| - 001.mp3  → Haupt-Sound für Spendenerkennung                                |
| - 002.mp3  → Optional: Weitere Sounds                                        |
| - 003.mp3  → Optional: Weitere Sounds                                        |
|                                                                              |
| Unterstützte Formate: MP3, WAV                                               |
| Bitrate: 8–320 kbps                                                          |
+------------------------------------------------------------------------------+

+------------------------------------------------------------------------------+
|                            ANIMATIONSABLAUF                                  |
+------------------------------------------------------------------------------+
| PHASE | DAUER   | EFFEKT                                                     |
+-------+---------+------------------------------------------------------------+
| 1     | 200 ms  | Weißer Blitz – alle LEDs leuchten weiß                     |
| 2     | 200 ms  | Regenbogen-Welle über alle LEDs                            |
| 3     | 400 ms  | Schnelle Farbrotation aller LEDs                           |
| ENDE  | –       | Zurück zur normalen Farbrotation                           |
+-------+---------+------------------------------------------------------------+
| Gesamt:   800 ms Animation + Sound-Effekt                                    |
| Debounce: 2 Sekunden Pause zwischen Animationen                              |
+------------------------------------------------------------------------------+
*/


#include <Arduino.h>
#include <FastLED.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// ============================================================================
//                              PIN DEFINITIONEN
// ============================================================================
#define SENSOR_PIN D2        // TCRT5000 Spenden-Sensor (HIGH = Spende erkannt)
#define DATA_PIN D3          // WS2812B LED-Strip Datenleitung
#define DFPLAYER_RX D5       // DFPlayer Mini RX (ESP8266 TX -> DFPlayer RX)
#define DFPLAYER_TX D6       // DFPlayer Mini TX (ESP8266 RX <- DFPlayer TX)

// ============================================================================
//                             LED KONFIGURATION
// ============================================================================
#define NUM_LEDS 6           // Anzahl LEDs im WS2812B Strip
#define LED_TYPE WS2812B     // FastLED Typ für WS2812B LEDs
#define BRIGHTNESS 255       // LED Helligkeit (0-255)
#define SATURATION 255       // LED Farbsättigung (0-255)

// ============================================================================
//                           HARDWARE INITIALISIERUNG
// ============================================================================
SoftwareSerial dfSerial(DFPLAYER_RX, DFPLAYER_TX);  // SoftwareSerial für DFPlayer
DFRobotDFPlayerMini dfPlayer;                        // DFPlayer Mini Objekt
CRGB leds[NUM_LEDS];                                 // LED Array für FastLED

// ============================================================================
//                            GLOBALE VARIABLEN
// ============================================================================
uint16_t color_pos = 0;                    // Aktuelle Farbposition für Rotation
unsigned long last_color_update = 0;      // Zeitstempel für Farbupdate
bool animation_active = false;             // Flag: Animation läuft
unsigned long animation_start = 0;         // Startzeitpunkt der Animation
uint8_t animation_step = 0;                // Aktueller Animationsschritt
unsigned long last_animation_trigger = 0; // Letzter Trigger für Debounce

// ============================================================================
//                              SETUP FUNKTION
// ============================================================================
void setup() {
  Serial.begin(115200);
  
  // Pin-Konfiguration
  pinMode(SENSOR_PIN, INPUT_PULLUP);          // Sensor mit Pull-up Widerstand
  
  // LED-Strip initialisieren
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setCorrection(CRGB(200, 40, 80)); // Farbkorrektur auf Weiß
  
  // DFPlayer Mini initialisieren
  dfSerial.begin(9600);
  Serial.println();
  Serial.println("+=============================================+");
  Serial.println("|        SPENDENBOX LED CONTROLLER            |");
  Serial.println("+=============================================+");
  Serial.println("Initialisiere DFPlayer Mini...");
  
  if (!dfPlayer.begin(dfSerial)) {
    Serial.println("FEHLER: DFPlayer Mini nicht verbunden!");
    Serial.println("Prüfen Sie:");
    Serial.println("1. Verkabelung (RX->D5, TX->D6, VCC->5V, GND->GND)");
    Serial.println("2. MicroSD-Karte eingesteckt");
    Serial.println("3. MP3-Dateien auf der SD-Karte");
  } else {
    Serial.println("DFPlayer Mini erfolgreich initialisiert!");
    dfPlayer.volume(25);                      // Lautstärke auf 25/30 setzen
    delay(100);
  }
  
  delay(1000);
  Serial.println("SpendenBox LED Controller bereit!");
  Serial.println("Warte auf Spenden...");
}

// ============================================================================
//                              MAIN LOOP
// ============================================================================
void loop() {
  // Animation läuft
  if (animation_active) {
    unsigned long animation_time = millis() - animation_start;
    
    if (animation_time < 200) {
      // Phase 1: Weißer Blitz (0-200ms)
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
      }
    } else if (animation_time < 400) {
      // Phase 2: Regenbogen-Welle (200-400ms)
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t hue = (animation_step * 20 + i * 40) % 256;
        leds[i] = CHSV(hue, 255, BRIGHTNESS);
      }
      animation_step++;
    } else if (animation_time < 800) {
      // Phase 3: Schnelle Farb-Rotation (400-800ms)
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((animation_step * 10) % 256, SATURATION, BRIGHTNESS);
      }
      animation_step += 3;
    } else {
      // Animation beenden
      animation_active = false;
      animation_step = 0;  // Animation-Step zurücksetzen
      Serial.println("Animation beendet - zurück zur Farbrotation");
    }
    
    FastLED.show();
    delay(50);
  } 
  // Normale Farbrotation im Standby
  else if (millis() - last_color_update > 50) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(color_pos, SATURATION, BRIGHTNESS);
    }
    FastLED.show();
    color_pos = (color_pos + 1) % 256;        // Farbposition inkrementieren
    last_color_update = millis();
  }

  // Spenden-Sensor Check mit 2-Sekunden Debounce
  if (digitalRead(SENSOR_PIN) == HIGH && !animation_active) {
    unsigned long current_time = millis();
    if (current_time - last_animation_trigger > 2000) {
      Serial.println("*** SPENDE ERKANNT! Animation und Sound starten! ***");
      
      // Sound abspielen (Datei 001.mp3 auf der SD-Karte)
      dfPlayer.play(1);
      
      // Animation starten
      animation_active = true;
      animation_start = millis();
      animation_step = 0;
      last_animation_trigger = current_time;
    }
  }
}