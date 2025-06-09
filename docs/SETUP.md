# 🎁 Donation Box - Schnellstart Setup

Willkommen zu deiner Donation Box! Dieses Setup hilft dir dabei, WiFi und MQTT in wenigen Minuten zu konfigurieren.

## 🚀 Setup in 3 einfachen Schritten

### 1️⃣ Setup-Script starten
```bash
python3 setup.py
```

### 2️⃣ Den Anweisungen folgen
Das Script führt dich durch:
- ✅ WiFi-Netzwerk konfigurieren
- ✅ MQTT-Broker einrichten 
- ✅ Automatische Datenschutz-Einstellungen

### 3️⃣ Firmware kompilieren und hochladen
```bash
# Projekt kompilieren
pio run

# Auf ESP32/ESP8266 hochladen
pio run --target upload

# Serial Monitor für Debugging
pio device monitor
```

## 📡 Was ist MQTT?

MQTT ermöglicht es dir, deine Donation Box remote zu überwachen:
- 📊 **Echzeit-Spenden-Benachrichtigungen**
- 📱 **Status-Updates auf dein Handy/Computer**
- 🌐 **Integration mit Smart Home Systemen**
- 📈 **Spenden-Statistiken sammeln**

## 🔒 Sicherheit

- ✅ Alle Zugangsdaten bleiben **nur auf deinem Computer**
- ✅ Automatischer **Git-Schutz** verhindert versehentliche Uploads
- ✅ **Keine Cloud-Verbindung** des Setup-Scripts

## ⚡ Schnellstart für Experten

Falls du schon weißt was du tust:

```bash
# Manuelle Konfiguration
cp include/credentials.h.example include/credentials.h
# Editiere credentials.h mit deinen Daten
nano include/credentials.h

# Kompilieren & Upload
pio run --target upload
```

## 🛠️ Unterstützte Hardware

- **ESP32-C3** (Seeed XIAO ESP32C3)
- **ESP8266** (Wemos D1 Mini)
- **WS2812B LED Strip** (6 LEDs)
- **TCRT5000 Sensor**

## 📞 Hilfe & Support

Bei Problemen:
1. 📋 Prüfe die Serial-Ausgabe: `pio device monitor`
2. 🔍 Lies die MQTT_GUIDE.md für Details
3. 🐛 Erstelle ein GitHub Issue bei Bugs

---

**Viel Spaß mit deiner Donation Box! 🎉**
