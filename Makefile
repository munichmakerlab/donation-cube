# Makefile für PlatformIO ESP8266 Projekt
# SpendenBox - FastLED Controller

# Umgebung aus platformio.ini
ENV = wemos_d1_mini

# PlatformIO Befehle
PIO = ~/.platformio/penv/bin/platformio

# Standard Ziel
.DEFAULT_GOAL := build

# Phony Targets (keine echten Dateien)
.PHONY: help build upload monitor clean test check install deps list-ports

# Hilfe anzeigen
help:
	@echo "Verfügbare Befehle:"
	@echo "  build      - Projekt kompilieren"
	@echo "  upload     - Firmware auf das Board hochladen"
	@echo "  monitor    - Serieller Monitor starten"
	@echo "  flash      - Build + Upload in einem Schritt"
	@echo "  clean      - Build-Dateien löschen"
	@echo "  test       - Tests ausführen"
	@echo "  check      - Code-Analyse durchführen"
	@echo "  install    - PlatformIO installieren/aktualisieren"
	@echo "  deps       - Abhängigkeiten installieren"
	@echo "  list-ports - Verfügbare serielle Ports anzeigen"
	@echo "  erase      - Flash-Speicher vollständig löschen"
	@echo "  size       - Speicherverbrauch anzeigen"

# Projekt kompilieren
build:
	@echo "Kompiliere Projekt..."
	$(PIO) run -e $(ENV)

# Firmware hochladen
upload:
	@echo "Lade Firmware hoch..."
	$(PIO) run -e $(ENV) --target upload

# Serieller Monitor
monitor:
	@echo "Starte seriellen Monitor..."
	$(PIO) device monitor

# Build und Upload in einem Schritt
flash: build upload monitor
	@echo "Build und Upload abgeschlossen!"

# Build-Dateien löschen
clean:
	@echo "Lösche Build-Dateien..."
	$(PIO) run --target clean
	@echo "Lösche .pio Ordner..."
	rm -rf .pio

# Tests ausführen
test:
	@echo "Führe Tests aus..."
	$(PIO) test -e $(ENV)

# Code-Analyse
check:
	@echo "Führe Code-Analyse durch..."
	$(PIO) check -e $(ENV)

# PlatformIO installieren/aktualisieren
install:
	@echo "Installiere/Aktualisiere PlatformIO..."
	pip install --upgrade platformio

# Abhängigkeiten installieren
deps:
	@echo "Installiere Projekt-Abhängigkeiten..."
	$(PIO) lib install

# Verfügbare serielle Ports anzeigen
list-ports:
	@echo "Verfügbare serielle Ports:"
	$(PIO) device list

# Flash-Speicher vollständig löschen
erase:
	@echo "Lösche Flash-Speicher..."
	$(PIO) run -e $(ENV) --target erase

# Speicherverbrauch anzeigen
size:
	@echo "Zeige Speicherverbrauch an..."
	$(PIO) run -e $(ENV) --target size

# Entwicklungs-Workflow: Clean -> Build -> Upload -> Monitor
dev: clean build upload monitor

# Release Build (mit Optimierungen)
release:
	@echo "Erstelle Release Build..."
	$(PIO) run -e $(ENV) --target upload --upload-port auto

# Debugging-Informationen
debug:
	@echo "PlatformIO Version:"
	$(PIO) --version
	@echo ""
	@echo "Projekt-Informationen:"
	$(PIO) project data
	@echo ""
	@echo "Board-Informationen:"
	$(PIO) boards $(ENV)

# Library-Verwaltung
lib-search:
	@read -p "Suchbegriff eingeben: " term; $(PIO) lib search $$term

lib-install:
	@read -p "Library Name/ID eingeben: " lib; $(PIO) lib install $$lib

lib-list:
	@echo "Installierte Libraries:"
	$(PIO) lib list

lib-update:
	@echo "Aktualisiere Libraries..."
	$(PIO) lib update

# Konfiguration anzeigen
config:
	@echo "Aktuelle Konfiguration:"
	@cat platformio.ini
	@echo ""
	@echo "Umgebungsdetails für $(ENV):"
	$(PIO) project config --environment $(ENV)

# OTA (Over-The-Air) Update (falls konfiguriert)
ota:
	@echo "OTA Update..."
	$(PIO) run -e $(ENV) --target upload --upload-port IP_ADDRESS

# Backup der aktuellen Firmware erstellen
backup:
	@echo "Erstelle Firmware-Backup..."
	@mkdir -p backup
	@cp .pio/build/$(ENV)/firmware.bin backup/firmware_$(shell date +%Y%m%d_%H%M%S).bin
	@echo "Backup erstellt in backup/"

# Alle Logs löschen
clean-logs:
	@echo "Lösche Log-Dateien..."
	@find . -name "*.log" -delete

# Schnelle Hilfe für häufige Befehle
quick:
	@echo "Schnelle Befehle:"
	@echo "  make build   - Kompilieren"
	@echo "  make flash   - Build + Upload + Monitor"
	@echo "  make monitor - Monitor starten"
	@echo "  make clean   - Aufräumen"
