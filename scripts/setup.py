#!/usr/bin/env python3
"""
Enhanced Setup script for Donation Box
Modular setup wizard with comprehensive workflow automation

Direct Actions:
  python setup.py                           # Interactive wizard
  python setup.py --config                  # Configure WiFi/MQTT only
  python setup.py --build                   # Build all targets
  python setup.py --build esp32c3           # Build specific target
  python setup.py --flash esp32c3           # Build and flash specific target
  python setup.py --monitor esp32c3         # Start serial monitor
  python setup.py --full                    # Complete setup workflow
  python setup.py --devices                 # List serial devices
"""

import sys
import argparse
from pathlib import Path

# Add setup module to path
setup_dir = Path(__file__).parent / "setup"
sys.path.insert(0, str(setup_dir))

# Import wizard and individual modules for direct actions
from wizard import main as wizard_main
from config import setup_credentials
from platformio_manager import setup_platformio_environment, parse_platformio_ini, select_build_targets
from builder import build_project_with_progress
from flasher import detect_serial_devices, list_all_serial_devices, flash_target_with_progress, start_serial_monitor
from ui import print_banner, colored_print, Colors

def build_action(target=None):
    """Direct build action"""
    colored_print("🔧 Setting up PlatformIO environment...", Colors.BLUE)
    pio_path = setup_platformio_environment()
    if not pio_path:
        colored_print("❌ Failed to setup PlatformIO", Colors.RED)
        return False
    
    colored_print("📋 Parsing project configuration...", Colors.BLUE)
    environments = parse_platformio_ini()
    if not environments:
        colored_print("❌ No build environments found", Colors.RED)
        return False
    
    # Select targets
    if target:
        # Build specific target
        target_env = None
        for env in environments:
            if env['name'] == target:
                target_env = env
                break
        
        if not target_env:
            colored_print(f"❌ Target '{target}' not found", Colors.RED)
            colored_print("Available targets:", Colors.YELLOW)
            for env in environments:
                colored_print(f"  • {env['name']} ({env['board']})", Colors.BLUE)
            return False
        
        selected_targets = [target_env]
    else:
        # Build all targets
        selected_targets = environments
    
    # Build targets
    colored_print(f"🔨 Building {len(selected_targets)} target(s)...", Colors.GREEN, bold=True)
    successful_builds = build_project_with_progress(selected_targets, pio_path)
    
    # Show results
    if successful_builds:
        colored_print(f"\n✅ Successfully built {len(successful_builds)}/{len(selected_targets)} targets:", Colors.GREEN, bold=True)
        for env in successful_builds:
            colored_print(f"   • {env['name']} ({env['board']})", Colors.GREEN)
        return True
    else:
        colored_print(f"\n❌ No builds successful", Colors.RED, bold=True)
        return False

def flash_action(target):
    """Direct flash action"""
    if not target:
        colored_print("❌ Target required for flashing", Colors.RED)
        return False
    
    # First build the target
    colored_print(f"🔨 Building {target} before flashing...", Colors.BLUE)
    if not build_action(target):
        return False
    
    # Setup PlatformIO
    pio_path = setup_platformio_environment()
    if not pio_path:
        return False
    
    # Parse environments to get target info
    environments = parse_platformio_ini()
    target_env = None
    for env in environments:
        if env['name'] == target:
            target_env = env
            break
    
    if not target_env:
        colored_print(f"❌ Target '{target}' not found", Colors.RED)
        return False
    
    # Flash the target
    return flash_target_with_progress(target_env, pio_path)

def monitor_action(target):
    """Direct serial monitor action"""
    if not target:
        colored_print("❌ Target required for monitoring", Colors.RED)
        return False
    
    # Setup PlatformIO
    pio_path = setup_platformio_environment()
    if not pio_path:
        return False
    
    # Parse environments to get target info
    environments = parse_platformio_ini()
    target_env = None
    for env in environments:
        if env['name'] == target:
            target_env = env
            break
    
    if not target_env:
        colored_print(f"❌ Target '{target}' not found", Colors.RED)
        return False
    
    # Start serial monitor
    return start_serial_monitor(target_env, pio_path)

def devices_action():
    """List all serial devices"""
    colored_print("🔍 Scanning for serial devices...", Colors.BLUE)
    devices = detect_serial_devices()
    
    if devices:
        colored_print(f"\n📱 Found {len(devices)} ESP device(s):", Colors.GREEN, bold=True)
        for device in devices:
            colored_print(f"   • {device['port']} - {device['description']}", Colors.GREEN)
    else:
        colored_print("\n❌ No ESP devices detected", Colors.YELLOW)
    
    # Also show all devices for debugging
    list_all_serial_devices()
    return len(devices) > 0

def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(
        description="Donation Box Setup Wizard",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python setup.py                    # Interactive wizard
  python setup.py --config           # Configure WiFi/MQTT only
  python setup.py --build            # Build all targets
  python setup.py --build esp32c3    # Build specific target
  python setup.py --flash esp32c3    # Build and flash specific target
  python setup.py --monitor esp32c3  # Start serial monitor
  python setup.py --full             # Complete setup workflow
  python setup.py --devices          # List serial devices
        """
    )
    
    # Action arguments (mutually exclusive)
    action_group = parser.add_mutually_exclusive_group()
    action_group.add_argument(
        '--config', action='store_true',
        help='Configure WiFi and MQTT credentials only'
    )
    action_group.add_argument(
        '--build', nargs='?', const='all', metavar='TARGET',
        help='Build project (optionally specify target: esp32c3, wemos_d1_mini)'
    )
    action_group.add_argument(
        '--flash', metavar='TARGET',
        help='Build and flash specific target (esp32c3, wemos_d1_mini)'
    )
    action_group.add_argument(
        '--monitor', metavar='TARGET',
        help='Start serial monitor for target (esp32c3, wemos_d1_mini)'
    )
    action_group.add_argument(
        '--full', action='store_true',
        help='Complete setup workflow (config + build + flash)'
    )
    action_group.add_argument(
        '--devices', action='store_true',
        help='List all detected serial devices'
    )
    
    return parser.parse_args()

def main():
    """Main entry point with argument parsing"""
    args = parse_arguments()
    
    # Print banner for all operations
    print_banner()
    
    # Handle direct actions
    if args.config:
        colored_print("🔧 Configuring WiFi and MQTT credentials...", Colors.CYAN, bold=True)
        return setup_credentials()
    
    elif args.build:
        target = None if args.build == 'all' else args.build
        return build_action(target)
    
    elif args.flash:
        return flash_action(args.flash)
    
    elif args.monitor:
        return monitor_action(args.monitor)
    
    elif args.full:
        colored_print("🚀 Starting complete setup workflow...", Colors.CYAN, bold=True)
        # Run wizard in full mode (will be implemented in wizard.py)
        return wizard_main(full_mode=True)
    
    elif args.devices:
        return devices_action()
    
    else:
        # No arguments - run interactive wizard
        colored_print("👋 Starting interactive setup wizard...", Colors.CYAN, bold=True)
        return wizard_main()

if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except KeyboardInterrupt:
        colored_print("\n\n👋 Setup cancelled by user", Colors.YELLOW, bold=True)
        sys.exit(0)
    except Exception as e:
        colored_print(f"\n❌ Unexpected error: {e}", Colors.RED, bold=True)
        sys.exit(1)