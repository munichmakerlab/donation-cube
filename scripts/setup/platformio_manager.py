#!/usr/bin/env python3
"""
PlatformIO management utilities
Handles installation, environment detection, and project configuration
"""

import os
import sys
import subprocess
import shutil
import configparser
from pathlib import Path
from ui import colored_print, Colors, show_progress_bar

def check_platformio_installed():
    """Check if PlatformIO is installed globally or locally"""
    # Check global installation
    if shutil.which('pio'):
        return True, None
    
    # Check local installation in scripts directory
    script_dir = Path(__file__).parent.parent
    local_pio = script_dir / '.venv' / 'bin' / 'pio'
    if local_pio.exists():
        return True, str(local_pio)
    
    return False, None

def install_platformio_locally():
    """Install PlatformIO locally in scripts directory"""
    script_dir = Path(__file__).parent.parent
    venv_dir = script_dir / '.venv'
    
    colored_print("📦 Installing PlatformIO locally...", Colors.BLUE, bold=True)
    
    try:
        # Create virtual environment
        colored_print("   Creating virtual environment...", Colors.BLUE)
        subprocess.run([sys.executable, '-m', 'venv', str(venv_dir)], 
                      check=True, capture_output=True)
        
        # Install PlatformIO
        colored_print("   Installing PlatformIO...", Colors.BLUE)
        pip_path = venv_dir / 'bin' / 'pip'
        subprocess.run([str(pip_path), 'install', 'platformio'], 
                      check=True, capture_output=True)
        
        pio_path = venv_dir / 'bin' / 'pio'
        colored_print(f"✅ PlatformIO installed locally: {pio_path}", Colors.GREEN)
        return str(pio_path)
        
    except subprocess.CalledProcessError as e:
        colored_print(f"❌ Failed to install PlatformIO: {e}", Colors.RED)
        colored_print("   Please install manually: pip install platformio", Colors.YELLOW)
        return None
    except Exception as e:
        colored_print(f"❌ Unexpected error: {e}", Colors.RED)
        return None

def setup_platformio_environment():
    """Ensure PlatformIO is available and return path"""
    colored_print("🔧 Checking PlatformIO installation...", Colors.CYAN, bold=True)
    
    # Check if already installed
    is_installed, pio_path = check_platformio_installed()
    
    if is_installed:
        if pio_path:
            colored_print(f"✅ Found PlatformIO (local): {pio_path}", Colors.GREEN)
        else:
            colored_print("✅ Found PlatformIO (global)", Colors.GREEN)
            pio_path = 'pio'
        return pio_path
    
    # Not installed, ask user what to do
    colored_print("⚠️  PlatformIO not found!", Colors.YELLOW, bold=True)
    colored_print("   PlatformIO is required to build and flash the firmware", Colors.YELLOW)
    print()
    
    while True:
        colored_print("🚀 Installation options:", Colors.CYAN, bold=True)
        colored_print("   [1] Install PlatformIO locally (recommended)", Colors.GREEN)
        colored_print("   [2] Skip - I'll install it manually", Colors.YELLOW)
        colored_print("   [q] Quit setup", Colors.RED)
        
        try:
            choice = input(f"\n{Colors.BOLD}Choose option [1/2/q]: {Colors.END}").strip().lower()
        except EOFError:
            colored_print("\n⚠️  EOF detected - skipping PlatformIO installation", Colors.YELLOW)
            colored_print("   Please install PlatformIO manually: pip install platformio", Colors.YELLOW)
            return None
        except KeyboardInterrupt:
            colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
            sys.exit(0)
        
        if choice == '1':
            pio_path = install_platformio_locally()
            if pio_path:
                return pio_path
            else:
                colored_print("❌ Local installation failed. Please install manually.", Colors.RED)
                return None
        elif choice == '2':
            colored_print("⚠️  Please install PlatformIO manually: pip install platformio", Colors.YELLOW)
            return None
        elif choice == 'q':
            colored_print("👋 Setup cancelled by user", Colors.YELLOW)
            sys.exit(0)
        else:
            colored_print("❌ Invalid choice. Please select 1, 2, or q.", Colors.RED)

def parse_platformio_ini():
    """Parse platformio.ini to get available environments"""
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    platformio_ini = script_dir / "platformio.ini"
    
    if not platformio_ini.exists():
        colored_print(f"❌ platformio.ini not found at {platformio_ini}", Colors.RED)
        return []
    
    try:
        config = configparser.ConfigParser()
        config.read(platformio_ini)
        
        environments = []
        for section in config.sections():
            if section.startswith('env:'):
                env_name = section[4:]  # Remove 'env:' prefix
                board = config.get(section, 'board', fallback='unknown')
                platform = config.get(section, 'platform', fallback='unknown')
                
                environments.append({
                    'name': env_name,
                    'board': board,
                    'platform': platform,
                    'section': section
                })
        
        colored_print(f"📋 Found {len(environments)} build environments:", Colors.GREEN)
        for env in environments:
            colored_print(f"   • {env['name']} ({env['board']} - {env['platform']})", Colors.BLUE)
        
        return environments
        
    except Exception as e:
        colored_print(f"❌ Error parsing platformio.ini: {e}", Colors.RED)
        return []

def select_build_targets(environments):
    """Let user select which targets to build"""
    if not environments:
        colored_print("❌ No build environments found", Colors.RED)
        return []
    
    colored_print("\n🎯 Select build targets:", Colors.CYAN, bold=True)
    
    # Show options
    colored_print("   [0] Build ALL targets", Colors.GREEN, bold=True)
    for i, env in enumerate(environments, 1):
        colored_print(f"   [{i}] {env['name']} ({env['board']})", Colors.BLUE)
    colored_print(f"   [{len(environments) + 1}] Skip building", Colors.YELLOW)
    
    while True:
        try:
            choice = input(f"\n{Colors.BOLD}Choose targets [0-{len(environments) + 1}]: {Colors.END}").strip()
        except EOFError:
            colored_print("\n⚠️  EOF detected - building all targets", Colors.YELLOW)
            return [env['name'] for env in environments]
        except KeyboardInterrupt:
            colored_print("\n👋 Setup cancelled by user", Colors.YELLOW)
            sys.exit(0)
        
        try:
            choice_num = int(choice)
            if choice_num == 0:
                # Build all
                return environments
            elif 1 <= choice_num <= len(environments):
                # Build specific target
                return [environments[choice_num - 1]]
            elif choice_num == len(environments) + 1:
                # Skip building
                return []
            else:
                colored_print(f"❌ Please enter a number between 0 and {len(environments) + 1}", Colors.RED)
        except ValueError:
            colored_print("❌ Please enter a valid number", Colors.RED)

def install_dependencies(pio_path):
    """Install project dependencies"""
    colored_print("📦 Installing project dependencies...", Colors.BLUE, bold=True)
    
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    
    try:
        # Run pio lib install
        result = subprocess.run(
            [pio_path, 'lib', 'install'],
            cwd=script_dir,
            capture_output=True,
            text=True,
            timeout=300  # 5 minute timeout
        )
        
        if result.returncode == 0:
            colored_print("✅ Dependencies installed successfully", Colors.GREEN)
            return True
        else:
            colored_print(f"❌ Failed to install dependencies:", Colors.RED)
            colored_print(f"   {result.stderr}", Colors.RED)
            return False
            
    except subprocess.TimeoutExpired:
        colored_print("❌ Dependency installation timed out", Colors.RED)
        return False
    except Exception as e:
        colored_print(f"❌ Error installing dependencies: {e}", Colors.RED)
        return False
