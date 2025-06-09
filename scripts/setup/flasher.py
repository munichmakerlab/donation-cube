#!/usr/bin/env python3
"""
Device flashing and monitoring utilities
Handles firmware upload and serial communication
"""

import subprocess
import threading
import re
import time
import serial.tools.list_ports
from pathlib import Path
from ui import colored_print, Colors, show_progress_bar, clear_progress_line

# Flash progress patterns  
FLASH_PATTERNS = {
    'detecting': r'Auto-detected: (.+)',
    'connecting': r'Connecting\.\.\.',
    'chip_info': r'Chip is (.+) \(revision (.+)\)',
    'features': r'Features: (.+)',
    'erasing': r'Flash will be erased from (.+) to (.+)',
    'writing': r'Writing at (.+)\.\.\. \((\d+) %\)',
    'verification': r'Hash of data verified',
    'reset': r'Hard resetting via RTS pin'
}

# Flash error patterns
FLASH_ERROR_PATTERNS = {
    'device_not_found': {
        'pattern': r'serial.serialutil.SerialException|could not open port',
        'message': 'Device connection failed',
        'solution': 'Check USB connection and device permissions'
    },
    'connection_timeout': {
        'pattern': r'Failed to connect|Timed out waiting for packet header',
        'message': 'Device connection timeout',
        'solution': 'Hold BOOT button and try again'
    },
    'permission_denied': {
        'pattern': r'Permission denied|Access is denied',
        'message': 'Permission denied accessing device',
        'solution': 'Add user to dialout group: sudo usermod -a -G dialout $USER'
    }
}

def select_target_for_flashing(successful_builds):
    """Let user select which target to flash"""
    if not successful_builds:
        colored_print("❌ No successful builds available for flashing", Colors.RED)
        return None
    
    if len(successful_builds) == 1:
        target = successful_builds[0]
        colored_print(f"📱 Auto-selecting target: {target['name']} ({target['board']})", Colors.GREEN)
        return target
    
    colored_print("\n📱 Select target to flash:", Colors.CYAN, bold=True)
    
    for i, env in enumerate(successful_builds, 1):
        colored_print(f"   [{i}] {env['name']} ({env['board']})", Colors.BLUE)
    colored_print(f"   [{len(successful_builds) + 1}] Skip flashing", Colors.YELLOW)
    
    while True:
        choice = input(f"\n{Colors.BOLD}Choose target [1-{len(successful_builds) + 1}]: {Colors.END}").strip()
        
        try:
            choice_num = int(choice)
            if 1 <= choice_num <= len(successful_builds):
                return successful_builds[choice_num - 1]
            elif choice_num == len(successful_builds) + 1:
                return None
            else:
                colored_print(f"❌ Please enter a number between 1 and {len(successful_builds) + 1}", Colors.RED)
        except ValueError:
            colored_print("❌ Please enter a valid number", Colors.RED)

def flash_target_with_progress(target_env, pio_path):
    """Flash firmware to target device with progress tracking"""
    if not target_env:
        colored_print("⚠️  No target selected for flashing", Colors.YELLOW)
        return False
    
    colored_print(f"\n⚡ Flashing {target_env['name']}...", Colors.CYAN, bold=True)
    colored_print(f"   Board: {target_env['board']}", Colors.BLUE)
    
    # Wait for device connection
    if not wait_for_device_connection():
        # Show debug information if device detection failed
        list_all_serial_devices()
        return False
    
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    cmd = [pio_path, 'run', '-e', target_env['name'], '--target', 'upload']
    
    try:
        # Start flash process
        process = subprocess.Popen(
            cmd,
            cwd=script_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
            bufsize=1
        )
        
        # Parse output in real-time
        progress = {'percentage': 0, 'stage': 'Starting'}
        output_lines = []
        
        # Start progress parsing thread
        parse_thread = threading.Thread(
            target=parse_flash_output_realtime,
            args=(process, target_env['name'], progress, output_lines)
        )
        parse_thread.start()
        
        # Wait for process to complete
        process.wait()
        parse_thread.join()
        
        # Check result
        if process.returncode == 0:
            clear_progress_line()
            show_progress_bar(f"✅ {target_env['name']}", 100, color=Colors.GREEN)
            colored_print(f"   Flashing successful!", Colors.GREEN, bold=True)
            return True
        else:
            clear_progress_line()
            show_progress_bar(f"❌ {target_env['name']}", progress['percentage'], color=Colors.RED)
            
            # Detect and show specific errors
            error_info = detect_flash_errors(output_lines)
            if error_info:
                show_flash_error(target_env['name'], error_info)
            else:
                colored_print(f"   Flashing failed with exit code {process.returncode}", Colors.RED)
            return False
            
    except Exception as e:
        clear_progress_line()
        colored_print(f"❌ Error flashing {target_env['name']}: {e}", Colors.RED)
        return False

def parse_flash_output_realtime(process, target_name, progress, output_lines):
    """Parse flash output in real-time and update progress"""
    while True:
        line = process.stdout.readline()
        if not line:
            break
        
        line = line.strip()
        output_lines.append(line)
        
        # Update progress based on line content
        update_flash_progress(line, progress, target_name)

def update_flash_progress(line, progress, target_name):
    """Update flash progress based on output line"""
    old_percentage = progress['percentage']
    
    # Device detection: 0-10%
    if re.search(FLASH_PATTERNS['detecting'], line):
        progress['percentage'] = 10
        progress['stage'] = 'Device detected'
    
    # Connecting: 10-20%
    elif re.search(FLASH_PATTERNS['connecting'], line):
        progress['percentage'] = 20
        progress['stage'] = 'Connecting'
    
    # Chip info: 20-30%
    elif re.search(FLASH_PATTERNS['chip_info'], line):
        progress['percentage'] = 30
        progress['stage'] = 'Chip identified'
    
    # Erasing: 30-40%
    elif re.search(FLASH_PATTERNS['erasing'], line):
        progress['percentage'] = 40
        progress['stage'] = 'Erasing flash'
    
    # Writing with percentage
    elif re.search(FLASH_PATTERNS['writing'], line):
        match = re.search(r'\((\d+) %\)', line)
        if match:
            write_percentage = int(match.group(1))
            # Map writing progress to 40-90% range
            progress['percentage'] = 40 + int(write_percentage * 0.5)
            progress['stage'] = f'Writing ({write_percentage}%)'
    
    # Verification: 90-95%
    elif re.search(FLASH_PATTERNS['verification'], line):
        progress['percentage'] = 95
        progress['stage'] = 'Verifying'
    
    # Reset: 95-100%
    elif re.search(FLASH_PATTERNS['reset'], line):
        progress['percentage'] = 100
        progress['stage'] = 'Complete'
    
    # Update display if progress changed
    if progress['percentage'] != old_percentage:
        clear_progress_line()
        show_progress_bar(
            f"{target_name} - {progress['stage']}", 
            progress['percentage']
        )

def detect_flash_errors(output_lines):
    """Detect specific flash errors and provide solutions"""
    for line in output_lines:
        for error_type, error_config in FLASH_ERROR_PATTERNS.items():
            if re.search(error_config['pattern'], line, re.IGNORECASE):
                return {
                    'type': error_type,
                    'message': error_config['message'],
                    'solution': error_config['solution'],
                    'line': line
                }
    return None

def show_flash_error(target_name, error_info):
    """Display detailed flash error information with solutions"""
    colored_print(f"\n🚨 Flash Error for {target_name}:", Colors.RED, bold=True)
    colored_print(f"   Issue: {error_info['message']}", Colors.RED)
    colored_print(f"   Solution: {error_info['solution']}", Colors.YELLOW)

def wait_for_device_connection(timeout=30):
    """Wait for device to be connected and ready"""
    colored_print("\n📱 Waiting for device connection...", Colors.BLUE, bold=True)
    colored_print("   Please connect your ESP32/ESP8266 via USB", Colors.BLUE)
    
    start_time = time.time()
    last_device_count = -1
    
    while time.time() - start_time < timeout:
        devices = detect_serial_devices()
        
        if len(devices) != last_device_count:
            last_device_count = len(devices)
            if devices:
                colored_print(f"   📍 Found {len(devices)} device(s):", Colors.GREEN)
                for device in devices:
                    colored_print(f"      • {device['port']} - {device['description']}", Colors.BLUE)
            else:
                colored_print("   🔍 No devices detected yet...", Colors.YELLOW)
        
        if devices:
            # Found at least one device
            time.sleep(1)  # Give it a moment to stabilize
            colored_print("✅ Device ready for flashing!", Colors.GREEN, bold=True)
            return True
        
        time.sleep(1)
    
    colored_print(f"❌ No device found within {timeout} seconds", Colors.RED)
    colored_print("   Please check USB connection and try again", Colors.YELLOW)
    
    # Offer manual flash option
    colored_print("\n💡 Alternative options:", Colors.CYAN, bold=True)
    colored_print("   [1] Try manual flash command", Colors.BLUE)
    colored_print("   [2] Skip flashing", Colors.BLUE)
    
    choice = input(f"\n{Colors.BOLD}Choose option [1/2]: {Colors.END}").strip()
    
    if choice == "1":
        colored_print("\n📋 Manual flash commands:", Colors.CYAN, bold=True)
        colored_print("   Connect your device and run:", Colors.BLUE)
        colored_print("   pio run -e esp32c3 --target upload", Colors.GREEN)
        colored_print("   # or: pio run -e wemos_d1_mini --target upload", Colors.GREEN)
        return False
    
    return False

def detect_serial_devices():
    """Detect available serial devices (ESP32/ESP8266)"""
    devices = []
    all_devices = []
    
    # ESP32/ESP8266 Vendor IDs (hexadecimal)
    esp_vendor_ids = [
        0x303A,  # Espressif Systems (ESP32)
        0x10C4,  # Silicon Labs (CP210x - common on ESP boards)
        0x1A86,  # QinHeng Electronics (CH340/CH341)
        0x0403,  # FTDI (FT232)
        0x067B,  # Prolific (PL2303)
    ]
    
    # Common ESP device description patterns
    esp_patterns = [
        'esp32', 'esp8266', 'jtag', 'debug unit', 'cp210x', 'ch340', 'ch341', 
        'ft232', 'pl2303', 'usb serial', 'silicon labs', 'prolific', 'ftdi', 
        'qinheng', 'usb-to-serial', 'uart', 'serial converter', 'usb2.0-serial'
    ]
    
    try:
        for port in serial.tools.list_ports.comports():
            port_info = {
                'port': port.device,
                'description': port.description or 'Unknown',
                'hwid': port.hwid or '',
                'vid': getattr(port, 'vid', None),
                'pid': getattr(port, 'pid', None)
            }
            
            all_devices.append(port_info)
            
            # Check if it's likely an ESP device or USB-to-Serial converter
            description_lower = port_info['description'].lower()
            hwid_lower = port_info['hwid'].lower()
            
            # Check by VID first (most reliable)
            is_esp_device = False
            if port_info['vid'] and port_info['vid'] in esp_vendor_ids:
                is_esp_device = True
            
            # Check by description patterns
            elif any(pattern in description_lower or pattern in hwid_lower 
                    for pattern in esp_patterns):
                is_esp_device = True
            
            # Check by device path patterns (Linux)
            elif (port.device.startswith('/dev/ttyUSB') or  # Linux USB serial
                  port.device.startswith('/dev/ttyACM') or  # Linux ACM devices  
                  port.device.startswith('COM')):           # Windows COM ports
                # For these device types, be more selective - need USB or serial in description
                if ('usb' in description_lower or 
                    'serial' in description_lower or
                    description_lower != 'n/a'):
                    is_esp_device = True
            
            if is_esp_device:
                devices.append(port_info)
        
        # Debug: Show all detected devices if none match ESP patterns
        if not devices and all_devices:
            colored_print("   🔍 Debug - All detected serial devices:", Colors.YELLOW)
            for device in all_devices:
                colored_print(f"      • {device['port']} - {device['description']}", Colors.BLUE)
                if device['hwid']:
                    colored_print(f"        HWID: {device['hwid']}", Colors.BLUE)
            colored_print("   💡 Tip: Try using any USB/Serial device above manually", Colors.CYAN)
        
    except Exception as e:
        colored_print(f"   ⚠️  Serial detection error: {e}", Colors.YELLOW)
    
    return devices

def start_serial_monitor(target_env, pio_path):
    """Start serial monitor for the flashed device"""
    if not target_env:
        return False
    
    colored_print(f"\n📺 Starting serial monitor for {target_env['name']}...", Colors.CYAN, bold=True)
    colored_print("   Press Ctrl+C to exit monitor", Colors.YELLOW)
    
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    cmd = [pio_path, 'device', 'monitor', '-e', target_env['name']]
    
    try:
        # Give user a moment to read the message
        time.sleep(2)
        
        # Start monitor (this will block until Ctrl+C)
        subprocess.run(cmd, cwd=script_dir)
        
        colored_print("\n📺 Serial monitor closed", Colors.BLUE)
        return True
        
    except KeyboardInterrupt:
        colored_print("\n📺 Serial monitor closed by user", Colors.BLUE)
        return True
    except Exception as e:
        colored_print(f"\n❌ Error starting serial monitor: {e}", Colors.RED)
        return False

def list_all_serial_devices():
    """Debug function to list all visible/real serial devices"""
    colored_print("\n🔍 All visible serial devices on system:", Colors.CYAN, bold=True)
    
    try:
        all_ports = list(serial.tools.list_ports.comports())
        
        if not all_ports:
            colored_print("   No serial devices found", Colors.YELLOW)
            return
        
        # Filter out virtual/dummy devices
        real_devices = []
        for port in all_ports:
            description = port.description or 'Unknown'
            hwid = port.hwid or ''
            
            # Skip devices with no useful information (virtual serial ports)
            if (description.lower() in ['n/a', 'unknown'] and 
                hwid.lower() in ['n/a', '', 'unknown']):
                continue
            
            # Skip standard virtual serial ports that have no hardware
            if (port.device.startswith('/dev/ttyS') and 
                description.lower() == 'n/a' and 
                not hwid):
                continue
                
            real_devices.append(port)
        
        if not real_devices:
            colored_print("   No visible/real serial devices found", Colors.YELLOW)
            colored_print("   (Virtual serial ports are hidden)", Colors.BLUE)
            return
        
        for port in real_devices:
            colored_print(f"   📍 {port.device}", Colors.GREEN, bold=True)
            colored_print(f"      Description: {port.description or 'Unknown'}", Colors.BLUE)
            colored_print(f"      Hardware ID: {port.hwid or 'Unknown'}", Colors.BLUE)
            if hasattr(port, 'vid') and port.vid:
                colored_print(f"      VID:PID: {port.vid:04X}:{port.pid:04X}", Colors.BLUE)
            colored_print("")
            
    except Exception as e:
        colored_print(f"   Error listing devices: {e}", Colors.RED)
