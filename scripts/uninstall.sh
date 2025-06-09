#!/bin/bash

# DonationBox Uninstallation Script
# Removes project and PlatformIO installation

set -e  # Exit on any error

echo "🗑️  DonationBox Uninstaller"
echo "==========================="

# Project configuration
PROJECT_NAME="donation-cube"

# Find and remove project directory
echo "📦 Removing project directory..."
if [ -d "$PROJECT_NAME" ]; then
    rm -rf "$PROJECT_NAME"
    echo "✅ Removed $PROJECT_NAME directory"
else
    echo "ℹ️  Project directory not found"
fi

# Remove PlatformIO installation
echo "🔧 Removing PlatformIO..."

# Remove PlatformIO via pip (if installed globally)
if command -v pio &> /dev/null; then
    echo "   Removing global PlatformIO installation..."
    pip uninstall -y platformio
    echo "✅ Removed global PlatformIO"
fi

# Remove PlatformIO home directory
if [ -d "$HOME/.platformio" ]; then
    echo "   Removing PlatformIO data directory..."
    rm -rf "$HOME/.platformio"
    echo "✅ Removed ~/.platformio"
fi

# Remove PlatformIO cache
if [ -d "$HOME/.cache/platformio" ]; then
    echo "   Removing PlatformIO cache..."
    rm -rf "$HOME/.cache/platformio"
    echo "✅ Removed PlatformIO cache"
fi

# Remove local PlatformIO binaries (if installed locally)
LOCAL_PLATFORMIO_PATHS=(
    "$HOME/.local/bin/pio"
    "$HOME/.local/bin/platformio"
    "/usr/local/bin/pio"
    "/usr/local/bin/platformio"
)

for path in "${LOCAL_PLATFORMIO_PATHS[@]}"; do
    if [ -f "$path" ]; then
        echo "   Removing $path..."
        rm -f "$path"
        echo "✅ Removed $path"
    fi
done

# Clean up any remaining PlatformIO packages
if command -v pip &> /dev/null; then
    echo "   Checking for remaining PlatformIO packages..."
    pip list | grep -i platformio | awk '{print $1}' | xargs -r pip uninstall -y 2>/dev/null || true
fi

echo ""
echo "🎉 Uninstallation complete!"
echo ""
echo "📋 What was removed:"
echo "   ✓ DonationBox project directory"
echo "   ✓ PlatformIO installation"
echo "   ✓ PlatformIO data and cache"
echo "   ✓ All related configuration files"
echo ""
echo "💡 If you installed PlatformIO system-wide with package manager,"
echo "   you may need to remove it manually with:"
echo "   • apt remove platformio (Ubuntu/Debian)"
echo "   • brew uninstall platformio (macOS)"
echo "   • pacman -R platformio (Arch Linux)"
echo ""