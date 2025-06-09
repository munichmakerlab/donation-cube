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
import urllib.request
import urllib.error
from pathlib import Path
from ui import colored_print, Colors, show_progress_bar

def check_platformio_installed():
    """Check if PlatformIO is installed globally or locally"""
    # Check global installation
    if shutil.which('pio'):
        return True, None
    
    # Check official PlatformIO installation locations
    home_dir = Path.home()
    possible_paths = [
        home_dir / ".platformio" / "penv" / "bin" / "pio",
        home_dir / ".platformio" / "penv" / "Scripts" / "pio.exe",  # Windows
        # Legacy local installation in scripts directory
        Path(__file__).parent.parent / '.venv' / 'bin' / 'pio'
    ]
    
    for pio_path in possible_paths:
        if pio_path.exists():
            return True, str(pio_path)
    
    return False, None

def install_platformio_locally():
    """Install PlatformIO using the official get-platformio.py script"""
    colored_print("📦 Installing PlatformIO using official installer...", Colors.BLUE, bold=True)
    
    try:
        import urllib.request
        import tempfile
        
        # Download get-platformio.py script
        colored_print("   Downloading PlatformIO installer...", Colors.BLUE)
        get_pio_url = "https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py"
        
        with tempfile.NamedTemporaryFile(mode='w+', suffix='.py', delete=False) as tmp_file:
            urllib.request.urlretrieve(get_pio_url, tmp_file.name)
            installer_path = tmp_file.name
        
        # Run the installer
        colored_print("   Running PlatformIO installer...", Colors.BLUE)
        result = subprocess.run([sys.executable, installer_path], 
                              capture_output=True, text=True, check=True)
        
        # Clean up temporary file
        os.unlink(installer_path)
        
        # Try to find the installed PlatformIO
        home_dir = Path.home()
        possible_paths = [
            home_dir / ".platformio" / "penv" / "bin" / "pio",
            home_dir / ".platformio" / "penv" / "Scripts" / "pio.exe",  # Windows
            "/usr/local/bin/pio",
            "/usr/bin/pio"
        ]
        
        for pio_path in possible_paths:
            if pio_path.exists():
                colored_print(f"✅ PlatformIO installed successfully: {pio_path}", Colors.GREEN)
                return str(pio_path)
        
        # If not found in common locations, try global command
        colored_print("✅ PlatformIO installed successfully (global)", Colors.GREEN)
        return "pio"
        
    except urllib.error.URLError as e:
        colored_print(f"❌ Failed to download installer: {e}", Colors.RED)
        colored_print("   Please check your internet connection", Colors.YELLOW)
        return None
    except subprocess.CalledProcessError as e:
        colored_print(f"❌ PlatformIO installation failed: {e}", Colors.RED)
        if e.stderr:
            colored_print(f"   Error details: {e.stderr}", Colors.RED)
        return None
    except Exception as e:
        colored_print(f"❌ Unexpected error during installation: {e}", Colors.RED)
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
    
    # Not installed, check if we're in non-interactive environment
    colored_print("⚠️  PlatformIO not found!", Colors.YELLOW, bold=True)
    
    # Check if we're in a non-interactive terminal
    if not sys.stdin.isatty() or not sys.stdout.isatty():
        colored_print("⚠️  Non-interactive environment detected", Colors.YELLOW)
        colored_print("   Attempting automatic PlatformIO installation...", Colors.BLUE)
        pio_path = install_platformio_locally()
        if pio_path:
            colored_print("✅ PlatformIO installed successfully", Colors.GREEN)
            return pio_path
        else:
            colored_print("❌ Automatic installation failed", Colors.RED)
            colored_print("   Please install PlatformIO manually: pip install platformio", Colors.YELLOW)
            return None
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
            colored_print("\n⚠️  EOF detected - attempting automatic PlatformIO installation", Colors.YELLOW)
            pio_path = install_platformio_locally()
            if pio_path:
                colored_print("✅ PlatformIO installed successfully", Colors.GREEN)
                return pio_path
            else:
                colored_print("❌ Automatic installation failed", Colors.RED)
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
            return environments  # Return environment dictionaries, not just names
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
