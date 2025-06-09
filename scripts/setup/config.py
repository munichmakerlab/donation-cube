#!/usr/bin/env python3
"""
Configuration management for WiFi and MQTT credentials
Handles credential input, validation, and file generation
"""

import os
import sys
import getpass
from pathlib import Path
from ui import colored_print, Colors

def is_interactive_terminal():
    """Check if we're running in an interactive terminal"""
    return sys.stdin.isatty() and sys.stdout.isatty()

def use_example_credentials():
    """Use example credentials as fallback for non-interactive environments"""
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    example_file = script_dir / "include" / "credentials.h.example"
    credentials_file = script_dir / "include" / "credentials.h"
    
    if example_file.exists():
        try:
            # Copy example file to credentials.h
            import shutil
            shutil.copy2(example_file, credentials_file)
            colored_print("✅ Using example credentials template", Colors.GREEN)
            colored_print("📝 Edit include/credentials.h to configure your WiFi/MQTT settings", Colors.BLUE)
            return True
        except Exception as e:
            colored_print(f"❌ Failed to copy example credentials: {e}", Colors.RED)
            return False
    else:
        colored_print("❌ Example credentials file not found", Colors.RED)
        return False

def get_wifi_credentials():
    """Get WiFi credentials from user input with EOF handling"""
    colored_print("\n📶 WiFi Configuration", Colors.BLUE, bold=True)
    colored_print("   Enter your WiFi network details for internet connectivity", Colors.BLUE)
    
    # Check if we're in an interactive environment first
    if not is_interactive_terminal():
        colored_print("⚠️  Non-interactive environment detected", Colors.YELLOW)
        colored_print("   Using placeholder values - please edit manually later", Colors.BLUE)
        return "YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD"
    
    print()
    
    # Get WiFi SSID with EOF handling
    while True:
        try:
            wifi_ssid = input(f"{Colors.BOLD}WiFi Network Name (SSID): {Colors.END}").strip()
            if wifi_ssid:
                break
            colored_print("❌ WiFi SSID cannot be empty. Please try again.", Colors.RED)
        except EOFError:
            colored_print("\n⚠️  EOF detected - using placeholder values", Colors.YELLOW)
            colored_print("   Please edit include/credentials.h manually", Colors.BLUE)
            return "YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD"
        except KeyboardInterrupt:
            colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
            sys.exit(0)
    
    # Get WiFi Password (hidden input) with EOF handling
    while True:
        try:
            wifi_password = getpass.getpass(f"{Colors.BOLD}WiFi Password: {Colors.END}")
            if wifi_password:
                break
            colored_print("❌ WiFi password cannot be empty. Please try again.", Colors.RED)
        except EOFError:
            colored_print("\n⚠️  EOF detected - using placeholder values", Colors.YELLOW)
            colored_print("   Please edit include/credentials.h manually", Colors.BLUE)
            return wifi_ssid, "YOUR_WIFI_PASSWORD"
        except KeyboardInterrupt:
            colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
            sys.exit(0)
    
    # Confirmation
    colored_print(f"\n✅ WiFi Network: {wifi_ssid}", Colors.GREEN)
    colored_print(f"✅ Password: {'*' * len(wifi_password)}", Colors.GREEN)
    
    return wifi_ssid, wifi_password

def get_mqtt_credentials():
    """Get MQTT broker credentials from user input with EOF handling"""
    colored_print("\n📡 MQTT Broker Configuration", Colors.BLUE, bold=True)
    colored_print("   MQTT enables donation tracking and remote monitoring", Colors.BLUE)
    
    # Check if we're in an interactive environment first
    if not is_interactive_terminal():
        colored_print("⚠️  Non-interactive environment detected", Colors.YELLOW)
        colored_print("   Using placeholder values - please edit manually later", Colors.BLUE)
        return "YOUR_MQTT_SERVER", 1883, "YOUR_MQTT_USER", "YOUR_MQTT_PASSWORD"
    
    print()
    
    # MQTT Server with EOF handling
    try:
        mqtt_server = input(f"{Colors.BOLD}MQTT Server (e.g., broker.hivemq.com): {Colors.END}").strip()
        if not mqtt_server:
            mqtt_server = "broker.hivemq.com"
            colored_print(f"   Using default: {mqtt_server}", Colors.YELLOW)
    except EOFError:
        colored_print("\n⚠️  EOF detected - using placeholder values", Colors.YELLOW)
        colored_print("   Please edit include/credentials.h manually", Colors.BLUE)
        return "YOUR_MQTT_SERVER", 1883, "YOUR_MQTT_USER", "YOUR_MQTT_PASSWORD"
    except KeyboardInterrupt:
        colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
        sys.exit(0)
    
    # MQTT Port with EOF handling
    mqtt_port = 1883
    while True:
        try:
            port_input = input(f"{Colors.BOLD}MQTT Port [1883]: {Colors.END}").strip()
            if not port_input:
                mqtt_port = 1883
                break
            try:
                mqtt_port = int(port_input)
                if 1 <= mqtt_port <= 65535:
                    break
                else:
                    colored_print("❌ Port must be between 1 and 65535", Colors.RED)
            except ValueError:
                colored_print("❌ Please enter a valid port number", Colors.RED)
        except EOFError:
            colored_print("\n⚠️  EOF detected - using default port 1883", Colors.YELLOW)
            mqtt_port = 1883
            break
        except KeyboardInterrupt:
            colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
            sys.exit(0)
    
    # MQTT Username (optional) with EOF handling
    mqtt_user = ""
    try:
        mqtt_user = input(f"{Colors.BOLD}MQTT Username (optional): {Colors.END}").strip()
    except EOFError:
        colored_print("\n⚠️  EOF detected - using empty username", Colors.YELLOW)
        mqtt_user = ""
    except KeyboardInterrupt:
        colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
        sys.exit(0)
    
    # MQTT Password (optional, only if username provided) with EOF handling
    mqtt_password = ""
    if mqtt_user:
        try:
            mqtt_password = getpass.getpass(f"{Colors.BOLD}MQTT Password: {Colors.END}")
        except EOFError:
            colored_print("\n⚠️  EOF detected - using empty password", Colors.YELLOW)
            mqtt_password = ""
        except KeyboardInterrupt:
            colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
            sys.exit(0)
    
    colored_print(f"\n✅ MQTT Server: {mqtt_server}:{mqtt_port}", Colors.GREEN)
    if mqtt_user:
        colored_print(f"✅ Username: {mqtt_user}", Colors.GREEN)
        colored_print(f"✅ Password: {'*' * len(mqtt_password)}", Colors.GREEN)
    else:
        colored_print("✅ No authentication (public broker)", Colors.GREEN)
    
    return mqtt_server, mqtt_port, mqtt_user, mqtt_password

def escape_string(s):
    """Escape special characters in strings for C++ code"""
    if not s:
        return '""'
    # Escape backslashes and quotes
    escaped = s.replace('\\', '\\\\').replace('"', '\\"')
    return f'"{escaped}"'

def create_credentials_file(wifi_ssid, wifi_password, mqtt_server, mqtt_port, mqtt_user, mqtt_password):
    """Create the credentials.h file with provided credentials"""
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    credentials_file = script_dir / "include" / "credentials.h"
    
    # Ensure include directory exists
    credentials_file.parent.mkdir(exist_ok=True)
    
    # Generate credentials content
    content = f'''#pragma once
// Auto-generated credentials file
// Generated by setup script on {os.popen("date").read().strip()}

// WiFi Configuration
#define WIFI_SSID {escape_string(wifi_ssid)}
#define WIFI_PASSWORD {escape_string(wifi_password)}

// MQTT Configuration
#define MQTT_SERVER {escape_string(mqtt_server)}
#define MQTT_PORT {mqtt_port}
#define MQTT_USER {escape_string(mqtt_user)}
#define MQTT_PASSWORD {escape_string(mqtt_password)}

// Device Configuration
#define CLIENT_ID_PREFIX "donation-box"  // Will be: donation-box-<MAC>
'''
    
    try:
        with open(credentials_file, 'w') as f:
            f.write(content)
        
        colored_print(f"✅ Created credentials file: {credentials_file}", Colors.GREEN)
        return True
        
    except Exception as e:
        colored_print(f"❌ Failed to create credentials file: {e}", Colors.RED)
        return False

def update_gitignore():
    """Add credentials.h to .gitignore if not already present"""
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    gitignore_file = script_dir / ".gitignore"
    
    try:
        # Read existing .gitignore
        gitignore_content = ""
        if gitignore_file.exists():
            with open(gitignore_file, 'r') as f:
                gitignore_content = f.read()
        
        # Check if credentials.h is already in .gitignore
        if "include/credentials.h" not in gitignore_content:
            # Add credentials.h to .gitignore
            with open(gitignore_file, 'a') as f:
                if not gitignore_content.endswith('\n'):
                    f.write('\n')
                f.write('# Credentials (auto-added by setup script)\n')
                f.write('include/credentials.h\n')
            
            colored_print("✅ Added credentials.h to .gitignore", Colors.GREEN)
        else:
            colored_print("✅ credentials.h already in .gitignore", Colors.GREEN)
            
    except Exception as e:
        colored_print(f"⚠️  Warning: Could not update .gitignore: {e}", Colors.YELLOW)

def check_credentials_exist():
    """Check if credentials file already exists"""
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    credentials_file = script_dir / "include" / "credentials.h"
    return credentials_file.exists()

def setup_credentials():
    """Complete credentials setup workflow"""
    colored_print("🔧 Setting up credentials...", Colors.CYAN, bold=True)
    
    # Check if we're in an interactive terminal
    if not is_interactive_terminal():
        colored_print("⚠️  Non-interactive terminal detected", Colors.YELLOW, bold=True)
        
        # Check if credentials already exist
        if check_credentials_exist():
            colored_print("✅ Using existing credentials.h file", Colors.GREEN)
            return True
        else:
            # Try to copy example file as fallback
            script_dir = Path(__file__).parent.parent.parent  # Go up to project root
            example_file = script_dir / "include" / "credentials.h.example"
            credentials_file = script_dir / "include" / "credentials.h"
            
            if example_file.exists():
                try:
                    import shutil
                    shutil.copy2(example_file, credentials_file)
                    colored_print("✅ Created credentials.h from example template", Colors.GREEN)
                    colored_print("⚠️  Using default credentials - please configure later:", Colors.YELLOW)
                    colored_print("   python3 scripts/setup.py --config", Colors.CYAN)
                    return True
                except Exception as e:
                    colored_print(f"❌ Failed to copy example file: {e}", Colors.RED)
            
            colored_print("❌ No credentials file found and terminal is non-interactive", Colors.RED)
            colored_print("   Please run setup in an interactive terminal to configure credentials:", Colors.YELLOW)
            colored_print("   python3 scripts/setup.py --config", Colors.CYAN)
            return False
    
    # Check if credentials already exist (interactive mode)
    if check_credentials_exist():
        colored_print("⚠️  Existing credentials.h found!", Colors.YELLOW, bold=True)
        while True:
            try:
                choice = input(f"{Colors.BOLD}Overwrite existing credentials? [y/N]: {Colors.END}").strip().lower()
                if choice in ['n', '', 'no']:
                    colored_print("✅ Keeping existing credentials", Colors.GREEN)
                    return True
                elif choice in ['y', 'yes']:
                    break
                else:
                    colored_print("❌ Please enter y or n", Colors.RED)
            except EOFError:
                colored_print("\n⚠️  EOF detected - keeping existing credentials", Colors.YELLOW)
                return True
            except KeyboardInterrupt:
                colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
                sys.exit(0)
    
    # Get credentials from user
    wifi_ssid, wifi_password = get_wifi_credentials()
    mqtt_server, mqtt_port, mqtt_user, mqtt_password = get_mqtt_credentials()
    
    # Create credentials file
    if not create_credentials_file(wifi_ssid, wifi_password, mqtt_server, mqtt_port, mqtt_user, mqtt_password):
        return False
    
    # Update .gitignore
    update_gitignore()
    
    # Print summary
    from .ui import print_summary
    print_summary(wifi_ssid, mqtt_server, mqtt_port, mqtt_user)
    
    return True
