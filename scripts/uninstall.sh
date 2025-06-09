#!/bin/bash

# DonationBox Uninstallation Script
# Removes project and PlatformIO installation

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

# Try to remove PlatformIO via pip (handle externally-managed-environment)
if command -v pio &> /dev/null; then
    echo "   Removing PlatformIO installation..."
    
    # Try different removal methods
    if pip uninstall -y platformio 2>/dev/null; then
        echo "✅ Removed PlatformIO via pip"
    elif pip uninstall -y platformio --break-system-packages 2>/dev/null; then
        echo "✅ Removed PlatformIO via pip (system override)"
    elif command -v pipx &> /dev/null && pipx uninstall platformio 2>/dev/null; then
        echo "✅ Removed PlatformIO via pipx"
    else
        echo "⚠️  Could not remove PlatformIO via package manager"
        echo "   Will remove files manually..."
    fi
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

# Remove local PlatformIO binaries
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

# Remove Python packages manually if pip failed
if [ -d "$HOME/.local/lib/python3"*/site-packages/platformio* ]; then
    echo "   Removing PlatformIO Python packages..."
    rm -rf "$HOME/.local/lib/python3"*/site-packages/platformio*
    echo "✅ Removed PlatformIO packages"
fi

# Check if pipx was used
if [ -d "$HOME/.local/share/pipx/venvs/platformio" ]; then
    echo "   Removing pipx PlatformIO environment..."
    rm -rf "$HOME/.local/share/pipx/venvs/platformio"
    echo "✅ Removed pipx PlatformIO"
fi

echo ""
echo "🎉 Uninstallation complete!"
echo ""
echo "📋 What was removed:"
echo "   ✓ DonationBox project directory"
echo "   ✓ PlatformIO data and cache (~/.platformio)"
echo "   ✓ PlatformIO binaries and packages"
echo "   ✓ All related configuration files"
echo ""

# Check if PlatformIO is still accessible
if command -v pio &> /dev/null; then
    echo "⚠️  PlatformIO command still accessible"
    echo "💡 If installed system-wide, remove manually with:"
    echo "   • sudo apt remove python3-platformio (Ubuntu/Debian)"
    echo "   • brew uninstall platformio (macOS)"
    echo "   • sudo pacman -R platformio (Arch Linux)"
else
    echo "✅ PlatformIO completely removed"
fi

echo ""