#!/usr/bin/env python3
"""
Main setup wizard orchestrator
Coordinates all setup components for the donation box project
"""

import sys
import signal
from pathlib import Path

# Import setup modules
from ui import print_banner, confirm_setup, print_final_success_message, print_final_manual_instructions, colored_print, Colors
from config import setup_credentials
from platformio_manager import setup_platformio_environment, parse_platformio_ini, select_build_targets, install_dependencies
from builder import build_project_with_progress
from flasher import select_target_for_flashing, flash_target_with_progress, start_serial_monitor

def signal_handler(sig, frame):
    """Handle Ctrl+C gracefully"""
    colored_print("\n\n👋 Setup cancelled by user", Colors.YELLOW, bold=True)
    sys.exit(0)

def setup_credentials_workflow():
    """Handle credentials setup workflow"""
    if setup_credentials():
        colored_print("✅ Credentials configured successfully!", Colors.GREEN, bold=True)
        return True
    else:
        colored_print("❌ Credentials setup failed", Colors.RED, bold=True)
        return False

def complete_setup_workflow():
    """Handle complete setup workflow"""
    success_stages = []
    
    # Stage 1: Credentials
    colored_print("\n🔑 Step 1: WiFi & MQTT Configuration", Colors.CYAN, bold=True)
    if setup_credentials():
        success_stages.append('credentials')
        colored_print("✅ Credentials configured successfully!", Colors.GREEN, bold=True)
    else:
        colored_print("❌ Credentials setup failed", Colors.RED, bold=True)
        # Ask if user wants to continue
        while True:
            cont = input(f"{Colors.BOLD}Continue without credentials? [y/N]: {Colors.END}").strip().lower()
            if cont in ['n', '', 'no']:
                colored_print("👋 Setup cancelled", Colors.YELLOW)
                return False
            elif cont in ['y', 'yes']:
                break
            else:
                colored_print("❌ Please enter y or n", Colors.RED)
    
    return build_and_flash_workflow()

def build_and_flash_workflow():
    """Handle build and flash workflow"""
    success_stages = []
    
    # Stage 1: PlatformIO setup
    colored_print("\n🔧 Step 1: PlatformIO Environment", Colors.CYAN, bold=True)
    pio_path = setup_platformio_environment()
    if not pio_path:
        colored_print("❌ PlatformIO setup failed", Colors.RED, bold=True)
        print_final_manual_instructions()
        return False
    
    success_stages.append('platformio')
    
    # Stage 2: Install dependencies
    if not install_dependencies(pio_path):
        colored_print("❌ Dependency installation failed", Colors.RED, bold=True)
        print_final_manual_instructions()
        return False
    
    success_stages.append('dependencies')
    
    # Stage 3: Parse environments and select targets
    environments = parse_platformio_ini()
    if not environments:
        colored_print("❌ No build environments found", Colors.RED, bold=True)
        return False
    
    build_targets = select_build_targets(environments)
    if not build_targets:
        colored_print("⚠️  No targets selected for building", Colors.YELLOW)
        return False
    
    # Stage 4: Build project
    successful_builds = build_project_with_progress(build_targets, pio_path)
    if not successful_builds:
        colored_print("❌ All builds failed", Colors.RED, bold=True)
        print_final_manual_instructions()
        return False
    
    success_stages.append('build')
    
    # Stage 5: Select and flash target
    flash_target = select_target_for_flashing(successful_builds)
    if not flash_target:
        colored_print("⚠️  No target selected for flashing", Colors.YELLOW)
        colored_print("✅ Build completed successfully - ready for manual flashing!", Colors.GREEN, bold=True)
        return True
    
    if flash_target_with_progress(flash_target, pio_path):
        success_stages.append('flash')
        
        # Stage 6: Start serial monitor
        colored_print("\n🎉 Device flashed successfully!", Colors.GREEN, bold=True)
        
        while True:
            monitor = input(f"{Colors.BOLD}Start serial monitor? [Y/n]: {Colors.END}").strip().lower()
            if monitor in ['', 'y', 'yes']:
                start_serial_monitor(flash_target, pio_path)
                break
            elif monitor in ['n', 'no']:
                colored_print("✅ Setup complete - serial monitor skipped", Colors.GREEN)
                break
            else:
                colored_print("❌ Please enter y or n", Colors.RED)
        
        success_stages.append('monitor')
        
        # Final success message
        print_final_success_message()
        return True
    else:
        colored_print("❌ Flashing failed", Colors.RED, bold=True)
        print_final_manual_instructions()
        return False

def main(full_mode=False):
    """Main setup workflow with continuous menu loop or direct full mode execution"""
    # Set up signal handler for graceful exit
    signal.signal(signal.SIGINT, signal_handler)
    
    # If full_mode is requested, run complete workflow directly
    if full_mode:
        return complete_setup_workflow()
    
    # Show banner once at startup
    print_banner()
    
    while True:
        # Show menu and get user choice
        choice = confirm_setup()
        
        if choice == 'q':
            colored_print("\n👋 Goodbye! Thanks for using the Donation Box Setup Wizard!", Colors.CYAN, bold=True)
            break
        elif choice == '1':  # Complete setup
            complete_setup_workflow()
        elif choice == '2':  # Build & flash only
            build_and_flash_workflow()
        elif choice == '3':  # Credentials only
            setup_credentials_workflow()
        
        # After any action, show a separator and wait for user input
        colored_print("\n" + "="*60, Colors.BLUE)
        input(f"{Colors.BOLD}Press Enter to return to main menu...{Colors.END}")
        print()  # Add some spacing before next menu

if __name__ == "__main__":
    main()
