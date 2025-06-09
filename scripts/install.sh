#!/bin/bash

# DonationBox Installation Script
# This script clones the repository and runs the setup wizard

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project configuration
REPO_URL="https://github.com/munichmakerlab/donation-cube.git"
PROJECT_NAME="donation-cube"

# Helper functions
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo -e "${BLUE}"
    echo "============================================"
    echo "    DonationBox Installation Script"
    echo "============================================"
    echo -e "${NC}"
}

check_dependencies() {
    print_info "Checking dependencies..."
    
    # Check if git is installed
    if ! command -v git &> /dev/null; then
        print_error "Git is not installed. Please install git first:"
        echo "  Ubuntu/Debian: sudo apt update && sudo apt install git"
        echo "  Fedora/RHEL:   sudo dnf install git"
        echo "  Arch:          sudo pacman -S git"
        exit 1
    fi
    
    # Check if python3 is installed
    if ! command -v python3 &> /dev/null; then
        print_error "Python3 is not installed. Please install python3 first:"
        echo "  Ubuntu/Debian: sudo apt update && sudo apt install python3 python3-pip"
        echo "  Fedora/RHEL:   sudo dnf install python3 python3-pip"
        echo "  Arch:          sudo pacman -S python python-pip"
        exit 1
    fi
    
    print_success "All dependencies are available"
}

clone_repository() {
    print_info "Cloning repository from ${REPO_URL}..."
    
    if [ -d "$PROJECT_NAME" ]; then
        print_warning "Directory '$PROJECT_NAME' already exists."
        read -p "Do you want to remove it and clone fresh? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            print_info "Removing existing directory..."
            rm -rf "$PROJECT_NAME"
        else
            print_info "Using existing directory..."
            return 0
        fi
    fi
    
    if git clone "$REPO_URL"; then
        print_success "Repository cloned successfully"
    else
        print_error "Failed to clone repository"
        exit 1
    fi
}

run_setup() {
    print_info "Changing to project directory..."
    cd "$PROJECT_NAME"
    
    if [ ! -f "scripts/setup.py" ]; then
        print_error "Setup script not found in scripts/setup.py"
        exit 1
    fi
    
    print_info "Starting project setup wizard..."
    echo ""
    
    if python3 scripts/setup.py --full; then
        print_success "Setup completed successfully!"
    else
        print_warning "Setup script finished with warnings or errors."
        echo ""
        print_info "You can run the setup again with:"
        echo "  cd $PROJECT_NAME"
        echo "  python3 scripts/setup.py"
        echo ""
        print_info "Or use individual commands:"
        echo "  python3 scripts/setup.py --config    # Configure WiFi/MQTT"
        echo "  python3 scripts/setup.py --build     # Build firmware"
        echo "  python3 scripts/setup.py --devices   # List devices"
        echo "  make help                             # Show all options"
    fi
}

show_next_steps() {
    echo ""
    print_success "DonationBox repository ready!"
    print_info "Next steps:"
    echo "1. Navigate to the project directory:"
    echo "   cd $PROJECT_NAME"
    echo ""
    echo "2. Start the interactive setup wizard:"
    echo "   python3 scripts/setup.py    # Complete guided setup"
    echo ""
    echo "3. Or use direct commands for specific tasks:"
    echo "   python3 scripts/setup.py --config        # Configure WiFi/MQTT"
    echo "   python3 scripts/setup.py --build         # Build firmware"
    echo "   python3 scripts/setup.py --flash esp32c3 # Flash to device"
    echo ""
    echo "4. Use Makefile for common development tasks:"
    echo "   make help      # Show all available commands"
    echo "   make setup     # Run setup wizard"
    echo "   make build     # Build project"
    echo "   make flash     # Build + Upload + Monitor"
    echo ""
    print_info "💡 Tip: The interactive setup wizard will guide you through:"
    echo "   • WiFi and MQTT configuration"
    echo "   • PlatformIO installation (if needed)"
    echo "   • Building firmware for your ESP device"
    echo "   • Flashing and monitoring your device"
    echo ""
    print_success "Happy coding with your DonationBox!"
}

# Main installation process
main() {
    print_header
    
    print_info "Starting DonationBox installation..."
    echo ""
    
    check_dependencies
    clone_repository
    run_setup
    show_next_steps
}

# Handle script interruption
trap 'print_error "Installation interrupted by user"; exit 1' INT

# Run main function
main "$@"
