# Makefile for PlatformIO ESP8266/ESP32 Project
# DonationBox - FastLED Controller

# Environment from platformio.ini
ENV = esp32c3 #wemos_d1_mini

# PlatformIO Commands
PIO = ~/.platformio/penv/bin/platformio

# Default Target
.DEFAULT_GOAL := build

# Phony Targets (not real files)
.PHONY: help build upload monitor clean test check install deps list-ports setup

# Show help
help:
	@echo "Available commands:"
	@echo "  setup      - Run project setup wizard"
	@echo "  build      - Compile project"
	@echo "  upload     - Upload firmware to board"
	@echo "  monitor    - Start serial monitor"
	@echo "  flash      - Build + Upload in one step"
	@echo "  clean      - Delete build files"
	@echo "  test       - Run tests"
	@echo "  check      - Perform code analysis"
	@echo "  install    - Install/update PlatformIO"
	@echo "  deps       - Install dependencies"
	@echo "  list-ports - Show available serial ports"
	@echo "  erase      - Completely erase flash memory"
	@echo "  size       - Show memory usage"

# Run project setup wizard
setup:
	@echo "Starting project setup wizard..."
	python3 scripts/setup.py

# Compile project
build:
	@echo "Compiling project..."
	$(PIO) run -e $(ENV)

# Upload firmware
upload:
	@echo "Uploading firmware..."
	$(PIO) run -e $(ENV) --target upload

# Serial monitor
monitor:
	@echo "Starting serial monitor..."
	$(PIO) device monitor

# Build and upload in one step
flash: build upload monitor
	@echo "Build and upload completed!"

# Delete build files
clean:
	@echo "Deleting build files..."
	$(PIO) run --target clean
	@echo "Deleting .pio folder..."
	rm -rf .pio

# Run tests
test:
	@echo "Running tests..."
	$(PIO) test -e $(ENV)

# Code analysis
check:
	@echo "Performing code analysis..."
	$(PIO) check -e $(ENV)

# Install/update PlatformIO
install:
	@echo "Installing/updating PlatformIO..."
	pip install --upgrade platformio

# Install dependencies
deps:
	@echo "Installing project dependencies..."
	$(PIO) lib install

# Show available serial ports
list-ports:
	@echo "Available serial ports:"
	$(PIO) device list

# Completely erase flash memory
erase:
	@echo "Erasing flash memory..."
	$(PIO) run -e $(ENV) --target erase

# Show memory usage
size:
	@echo "Showing memory usage..."
	$(PIO) run -e $(ENV) --target size

# Development workflow: Clean -> Build -> Upload -> Monitor
dev: clean build upload monitor

# Release build (with optimizations)
release:
	@echo "Creating release build..."
	$(PIO) run -e $(ENV) --target upload --upload-port auto

# Debugging information
debug:
	@echo "PlatformIO Version:"
	$(PIO) --version
	@echo ""
	@echo "Project Information:"
	$(PIO) project data
	@echo ""
	@echo "Board Information:"
	$(PIO) boards $(ENV)

# Library management
lib-search:
	@read -p "Enter search term: " term; $(PIO) lib search $$term

lib-install:
	@read -p "Enter library name/ID: " lib; $(PIO) lib install $$lib

lib-list:
	@echo "Installed libraries:"
	$(PIO) lib list

lib-update:
	@echo "Updating libraries..."
	$(PIO) lib update

# Show configuration
config:
	@echo "Current configuration:"
	@cat platformio.ini
	@echo ""
	@echo "Environment details for $(ENV):"
	$(PIO) project config --environment $(ENV)

# OTA (Over-The-Air) Update (if configured)
ota:
	@echo "OTA Update..."
	$(PIO) run -e $(ENV) --target upload --upload-port IP_ADDRESS

# Create backup of current firmware
backup:
	@echo "Creating firmware backup..."
	@mkdir -p backup
	@cp .pio/build/$(ENV)/firmware.bin backup/firmware_$(shell date +%Y%m%d_%H%M%S).bin
	@echo "Backup created in backup/"

# Delete all logs
clean-logs:
	@echo "Deleting log files..."
	@find . -name "*.log" -delete

# Quick help for common commands
quick:
	@echo "Quick commands:"
	@echo "  make setup   - Project setup script"
	@echo "  make build   - Compile"
	@echo "  make flash   - Build + Upload + Monitor"
	@echo "  make monitor - Start monitor"
	@echo "  make clean   - Clean up"
