#!/bin/bash

# DonationBox Installation Script
# Simple: Clone repo and run setup

set -e  # Exit on any error

# Project configuration
REPO_URL="https://github.com/munichmakerlab/donation-cube.git"
PROJECT_NAME="donation-cube"

echo "🎁 DonationBox Installation"
echo "=========================="

# Check basic dependencies
if ! command -v git &> /dev/null; then
    echo "❌ Git not found. Please install git first."
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo "❌ Python3 not found. Please install python3 first."
    exit 1
fi

# Clone repository
echo "📦 Cloning repository..."
if [ -d "$PROJECT_NAME" ]; then
    echo "⚠️  Directory '$PROJECT_NAME' exists - removing..."
    rm -rf "$PROJECT_NAME"
fi

git clone "$REPO_URL"
cd "$PROJECT_NAME"

# Run setup - simple execution without error handling
echo "🚀 Starting setup..."
exec python3 scripts/setup.py