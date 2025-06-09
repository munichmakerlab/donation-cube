#!/usr/bin/env python3
"""
Build system with real-time progress parsing and error detection
Handles PlatformIO build process with enhanced output visualization
"""

import subprocess
import threading
import re
import time
from pathlib import Path
from ui import colored_print, Colors, show_progress_bar, clear_progress_line

# Build progress patterns
BUILD_PATTERNS = {
    'dependency': r'Installing dependencies|Resolving (.+) dependencies',
    'compiling': r'Compiling (.+\.cpp|\.c)|Building (.+)',
    'linking': r'Linking (.+\.elf)|Creating (.+\.bin)',
    'generating': r'Successfully created (.+\.bin)|Building (.+\.bin)',
    'completed': r'=+ \[SUCCESS\]|BUILD SUCCESSFUL'
}

# Error patterns with solutions
ERROR_PATTERNS = {
    'missing_library': {
        'pattern': r'fatal error: (.+\.h): No such file',
        'message': 'Missing library: {}',
        'solution': 'Install required library via PlatformIO'
    },
    'compilation_error': {
        'pattern': r'error: (.+)',
        'message': 'Code compilation error: {}',
        'solution': 'Fix code errors and try again'
    },
    'permission_denied': {
        'pattern': r'Permission denied|Access is denied',
        'message': 'Permission denied accessing files',
        'solution': 'Check file permissions or run with appropriate rights'
    }
}

def build_project_with_progress(environments, pio_path):
    """Build project with real-time progress visualization"""
    if not environments:
        colored_print("⚠️  No environments selected for building", Colors.YELLOW)
        return []
    
    colored_print(f"\n🔨 Building {len(environments)} target(s)...", Colors.CYAN, bold=True)
    
    successful_builds = []
    script_dir = Path(__file__).parent.parent.parent  # Go up to project root
    
    for i, env in enumerate(environments, 1):
        colored_print(f"\n📦 Building {env['name']} ({i}/{len(environments)})", Colors.BLUE, bold=True)
        colored_print(f"   Board: {env['board']} | Platform: {env['platform']}", Colors.BLUE)
        
        # Build command
        cmd = [pio_path, 'run', '-e', env['name']]
        
        try:
            # Start build process
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
                target=parse_build_output_realtime,
                args=(process, env['name'], progress, output_lines)
            )
            parse_thread.start()
            
            # Wait for process to complete
            process.wait()
            parse_thread.join()
            
            # Check result
            if process.returncode == 0:
                clear_progress_line()
                show_progress_bar(f"✅ {env['name']}", 100, color=Colors.GREEN)
                colored_print(f"   Build successful!", Colors.GREEN)
                successful_builds.append(env)
            else:
                clear_progress_line()
                show_progress_bar(f"❌ {env['name']}", progress['percentage'], color=Colors.RED)
                
                # Detect and show specific errors
                error_info = detect_build_errors(output_lines)
                if error_info:
                    show_build_error(env['name'], error_info)
                else:
                    colored_print(f"   Build failed with exit code {process.returncode}", Colors.RED)
                    
        except Exception as e:
            clear_progress_line()
            colored_print(f"❌ Error building {env['name']}: {e}", Colors.RED)
    
    # Summary
    print()
    if successful_builds:
        colored_print(f"✅ Successfully built {len(successful_builds)}/{len(environments)} targets:", Colors.GREEN, bold=True)
        for env in successful_builds:
            colored_print(f"   • {env['name']} ({env['board']})", Colors.GREEN)
    else:
        colored_print("❌ No builds were successful", Colors.RED, bold=True)
    
    return successful_builds

def parse_build_output_realtime(process, env_name, progress, output_lines):
    """Parse build output in real-time and update progress"""
    while True:
        line = process.stdout.readline()
        if not line:
            break
        
        line = line.strip()
        output_lines.append(line)
        
        # Update progress based on line content
        update_build_progress(line, progress, env_name)

def update_build_progress(line, progress, env_name):
    """Update build progress based on output line"""
    old_percentage = progress['percentage']
    
    # Dependency resolution: 0-15%
    if re.search(BUILD_PATTERNS['dependency'], line, re.IGNORECASE):
        progress['percentage'] = min(15, progress['percentage'] + 2)
        progress['stage'] = 'Dependencies'
    
    # Compiling: 15-70%
    elif re.search(BUILD_PATTERNS['compiling'], line, re.IGNORECASE):
        progress['percentage'] = max(15, min(70, progress['percentage'] + 3))
        progress['stage'] = 'Compiling'
    
    # Linking: 70-85%
    elif re.search(BUILD_PATTERNS['linking'], line, re.IGNORECASE):
        progress['percentage'] = max(70, min(85, progress['percentage'] + 5))
        progress['stage'] = 'Linking'
    
    # Generating: 85-95%
    elif re.search(BUILD_PATTERNS['generating'], line, re.IGNORECASE):
        progress['percentage'] = max(85, min(95, progress['percentage'] + 5))
        progress['stage'] = 'Generating'
    
    # Completed: 100%
    elif re.search(BUILD_PATTERNS['completed'], line, re.IGNORECASE):
        progress['percentage'] = 100
        progress['stage'] = 'Complete'
    
    # Update display if progress changed
    if progress['percentage'] != old_percentage:
        clear_progress_line()
        show_progress_bar(
            f"{env_name} - {progress['stage']}", 
            progress['percentage']
        )

def detect_build_errors(output_lines):
    """Detect specific build errors and provide solutions"""
    for line in output_lines:
        for error_type, error_config in ERROR_PATTERNS.items():
            match = re.search(error_config['pattern'], line, re.IGNORECASE)
            if match:
                return {
                    'type': error_type,
                    'message': error_config['message'].format(match.group(1) if match.groups() else ''),
                    'solution': error_config['solution'],
                    'line': line
                }
    return None

def show_build_error(env_name, error_info):
    """Display detailed error information with solutions"""
    colored_print(f"\n🚨 Build Error in {env_name}:", Colors.RED, bold=True)
    colored_print(f"   Issue: {error_info['message']}", Colors.RED)
    colored_print(f"   Solution: {error_info['solution']}", Colors.YELLOW)
    
    if 'line' in error_info:
        colored_print(f"   Details: {error_info['line']}", Colors.BLUE)
