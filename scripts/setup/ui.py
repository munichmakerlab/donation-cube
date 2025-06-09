#!/usr/bin/env python3
"""
UI utilities for the setup wizard
Provides colors, progress bars, banners, and user interaction
"""

import os
import time

class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'
    
    # Background colors
    BG_GREEN = '\033[42m'
    BG_RED = '\033[41m'
    BG_YELLOW = '\033[43m'

def show_progress_bar(label, percentage, width=20, color=Colors.GREEN):
    """Display animated progress bar"""
    filled = int(width * percentage / 100)
    bar = '█' * filled + '░' * (width - filled)
    colored_print(f"   {label:<30} {bar} {percentage:3d}%", color)

def clear_progress_line():
    """Clear the current line for progress updates"""
    print('\r' + ' ' * 80 + '\r', end='', flush=True)

def colored_print(text, color=Colors.END, bold=False):
    """Print colored text"""
    style = Colors.BOLD if bold else ""
    print(f"{style}{color}{text}{Colors.END}")

def print_banner():
    """Print beautiful setup banner with ASCII art"""
    os.system('clear' if os.name == 'posix' else 'cls')  # Clear screen
    
    # ASCII Art Banner
    colored_print("", Colors.CYAN)
    colored_print("╔══════════════════════════════════════════════════════════════════╗", Colors.CYAN, bold=True)
    colored_print("║                                                                  ║", Colors.CYAN, bold=True)
    colored_print("║            🎁  DONATION BOX - SETUP WIZARD  🎁                   ║", Colors.CYAN, bold=True)
    colored_print("║                                                                  ║", Colors.CYAN, bold=True)
    colored_print("║     📡 WiFi & MQTT Config • 🔧 PlatformIO • ⚡ Build & Flash     ║", Colors.CYAN, bold=True)
    colored_print("║                                                                  ║", Colors.CYAN, bold=True)
    colored_print("╚══════════════════════════════════════════════════════════════════╝", Colors.CYAN, bold=True)
    print()
    
    # Welcome message
    colored_print("👋 Welcome to the Complete Setup Wizard!", Colors.GREEN, bold=True)
    colored_print("   From configuration to deployment - all in one place!", Colors.BLUE)
    print()
    
    # What this script does
    colored_print("📋 What this wizard does:", Colors.YELLOW, bold=True)
    colored_print("   ✓ Configure WiFi & MQTT credentials", Colors.GREEN)
    colored_print("   ✓ Install PlatformIO (if needed)", Colors.GREEN)
    colored_print("   ✓ Build project for all ESP targets", Colors.GREEN)
    colored_print("   ✓ Flash firmware to your device", Colors.GREEN)
    colored_print("   ✓ Start serial monitor automatically", Colors.GREEN)
    print()

def show_menu():
    """Show the main menu options"""
    colored_print("⚠️  SETUP OPTIONS:", Colors.YELLOW, bold=True)
    colored_print("   You can configure credentials or skip directly to PlatformIO build", Colors.YELLOW)
    print()
    
    colored_print("🔒 Privacy & Security:", Colors.BLUE, bold=True)
    colored_print("   ✓ Credentials are stored locally only", Colors.GREEN)
    colored_print("   ✓ Automatic protection from Git commits", Colors.GREEN)
    colored_print("   ✓ No transmission to external servers", Colors.GREEN)
    print()
    
    colored_print("🚀 What do you want to do?", Colors.CYAN, bold=True)
    colored_print("   [1] Complete setup (WiFi + MQTT + Build + Flash)", Colors.GREEN)
    colored_print("   [2] Skip credentials - just build & flash", Colors.YELLOW)
    colored_print("   [3] Only configure credentials", Colors.BLUE)
    colored_print("   [q] Quit", Colors.RED)

def confirm_setup():
    """Ask user if they want to proceed with setup"""
    show_menu()
    
    while True:
        try:
            choice = input(f"\n{Colors.BOLD}Enter your choice [1/2/3/q]: {Colors.END}").strip().lower()
            
            if choice in ['1', '2', '3', 'q']:
                return choice
            
            colored_print("\n❌ Invalid choice. Please select 1, 2, 3, or q.", Colors.RED)
            time.sleep(1)
            # Show menu again after invalid input
            print()
        except EOFError:
            colored_print("\n⚠️  EOF detected - exiting setup", Colors.YELLOW)
            return 'q'
        except KeyboardInterrupt:
            colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
            return 'q'
        show_menu()

def print_summary(wifi_ssid, mqtt_server, mqtt_port, mqtt_user):
    """Print configuration summary"""
    colored_print("\n" + "="*60, Colors.CYAN, bold=True)
    colored_print("                 CONFIGURATION SUMMARY", Colors.CYAN, bold=True)
    colored_print("="*60, Colors.CYAN, bold=True)
    
    colored_print(f"📶 WiFi Network:     {wifi_ssid}", Colors.GREEN)
    colored_print(f"📡 MQTT Server:      {mqtt_server}:{mqtt_port}", Colors.GREEN)
    colored_print(f"👤 MQTT User:        {mqtt_user}", Colors.GREEN)
    colored_print(f"🔐 Password:         {'*' * 8} (hidden)", Colors.GREEN)
    
    colored_print("\n✅ Configuration saved successfully!", Colors.GREEN, bold=True)
    colored_print("📁 File: include/credentials.h", Colors.BLUE)
    print()

def print_final_success_message():
    """Print final success message"""
    colored_print("\n" + "="*60, Colors.GREEN, bold=True)
    colored_print("🎉              SETUP COMPLETED SUCCESSFULLY!              🎉", Colors.GREEN, bold=True)
    colored_print("="*60, Colors.GREEN, bold=True)
    
    colored_print("\n✅ Your donation box is now ready and running!", Colors.GREEN, bold=True)
    colored_print("   📡 Connected to WiFi and MQTT", Colors.GREEN)
    colored_print("   💡 LEDs should show breathing animation", Colors.GREEN)
    colored_print("   👋 Wave your hand to test donation detection", Colors.GREEN)
    print()
    
    colored_print("🛠️  Next steps:", Colors.YELLOW, bold=True)
    colored_print("   • Test donation detection by waving hand over sensor", Colors.BLUE)
    colored_print("   • Check MQTT broker for donation messages", Colors.BLUE)
    colored_print("   • Monitor serial output for debugging", Colors.BLUE)
    print()
    
    colored_print("📚 Documentation:", Colors.CYAN, bold=True)
    colored_print("   • README.md - Project overview and troubleshooting", Colors.BLUE)
    colored_print("   • docs/MQTT.md - MQTT integration details", Colors.BLUE)
    print()
    
    colored_print("🎁 Enjoy your modular donation box! 🎁", Colors.GREEN, bold=True)

def print_final_manual_instructions():
    """Print manual setup instructions if automated setup fails"""
    colored_print("\n📋 Manual Setup Instructions:", Colors.YELLOW, bold=True)
    colored_print("   If you need to set up manually, run these commands:", Colors.YELLOW)
    print()
    
    colored_print("1. Install PlatformIO:", Colors.BLUE, bold=True)
    colored_print("   pip install platformio", Colors.CYAN)
    print()
    
    colored_print("2. Build and flash:", Colors.BLUE, bold=True)
    colored_print("   pio run -e esp32c3 --target upload", Colors.CYAN)
    colored_print("   # or for ESP8266: pio run -e wemos_d1_mini --target upload", Colors.CYAN)
    print()
