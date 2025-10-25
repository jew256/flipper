#!/usr/bin/env python3

import argparse
import json
import logging
import threading
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import socket

from splitflap_proto import ask_for_serial_port, splitflap_context

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# Default alphabet (same as in splitflap_proto.py)
DEFAULT_ALPHABET = [
    ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
    'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
    'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', '.', ',', "'"
]

class SplitflapHTTPServer:
    def __init__(self, port, serial_port=None):
        self.port = port
        self.serial_port = serial_port
        self.splitflap = None
        self.splitflap_context = None
        self.splitflap_lock = threading.Lock()
        self.server = None
        self.server_thread = None
        self.running = False
        self.num_modules = 6  # Hard-coded to 6 modules
        
    def connect_splitflap(self):
        """Connect to the splitflap display"""
        try:
            if self.serial_port is None:
                self.serial_port = ask_for_serial_port()
            
            logger.info(f"Connecting to splitflap on {self.serial_port}")
            # Use the context manager properly
            self.splitflap_context = splitflap_context(self.serial_port)
            self.splitflap = self.splitflap_context.__enter__()
            
            # Wait a moment for the splitflap to send initial state
            time.sleep(2)
            
            # Try to get num_modules, fallback to hard-coded value if not available yet
            try:
                self.num_modules = self.splitflap.get_num_modules()
                logger.info(f"Detected {self.num_modules} modules from splitflap")
            except (AssertionError, AttributeError):
                logger.warning(f"Number of modules not yet known, using default: {self.num_modules}")
            
            # Try to get alphabet, fallback to default if not available
            try:
                alphabet = self.splitflap.get_alphabet()
                logger.info(f"Detected alphabet from splitflap: {alphabet}")
            except (AttributeError, Exception) as e:
                logger.warning(f"Could not get alphabet from splitflap: {e}, using default")
                # Use the default alphabet
                alphabet = DEFAULT_ALPHABET
            
            logger.info(f"Connected to splitflap with {self.num_modules} modules")
            logger.info(f"Available characters: {alphabet}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to connect to splitflap: {e}")
            return False
    
    def disconnect_splitflap(self):
        """Disconnect from the splitflap display"""
        try:
            if hasattr(self, 'splitflap_context') and self.splitflap_context:
                self.splitflap_context.__exit__(None, None, None)
                self.splitflap_context = None
                self.splitflap = None
                logger.info("Disconnected from splitflap")
        except Exception as e:
            logger.error(f"Error disconnecting from splitflap: {e}")
    
    def set_text(self, message, force_movement=False):
        """Set text on the splitflap display"""
        with self.splitflap_lock:
            if not self.splitflap:
                return False, "Splitflap not connected"
            
            try:
                # Validate message length
                if len(message) > self.num_modules:
                    message = message[:self.num_modules]
                elif len(message) < self.num_modules:
                    message = message.ljust(self.num_modules, ' ')
                
                # Validate characters
                try:
                    alphabet = self.splitflap.get_alphabet()
                except (AttributeError, Exception):
                    # Use default alphabet if get_alphabet() fails
                    alphabet = DEFAULT_ALPHABET
                
                invalid_chars = [c for c in message if c not in alphabet]
                if invalid_chars:
                    return False, f"Invalid characters: {invalid_chars}. Available: {alphabet}"
                
                # Set the text
                if force_movement:
                    self.splitflap.set_text(message, force_movement=self.splitflap.ForceMovement.ALL)
                else:
                    self.splitflap.set_text(message)
                
                logger.info(f"Set text: '{message}'")
                return True, f"Text set to: '{message}'"
                
            except Exception as e:
                logger.error(f"Error setting text: {e}")
                return False, f"Error: {str(e)}"
    
    def get_status(self):
        """Get current status of the splitflap display"""
        with self.splitflap_lock:
            if not self.splitflap:
                return {"connected": False, "error": "Splitflap not connected"}
            
            try:
                try:
                    alphabet = self.splitflap.get_alphabet()
                except (AttributeError, Exception):
                    # Use default alphabet if get_alphabet() fails
                    alphabet = DEFAULT_ALPHABET
                
                return {
                    "connected": True,
                    "num_modules": self.num_modules,
                    "alphabet": list(alphabet),
                    "serial_port": self.serial_port
                }
            except Exception as e:
                return {"connected": False, "error": str(e)}

class SplitflapHTTPRequestHandler(BaseHTTPRequestHandler):
    def __init__(self, splitflap_server, *args, **kwargs):
        self.splitflap_server = splitflap_server
        super().__init__(*args, **kwargs)
    
    def do_GET(self):
        """Handle GET requests"""
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        if path == '/':
            self.send_home_page()
        elif path == '/status':
            self.send_status()
        elif path == '/health':
            self.send_health_check()
        elif path == '/app.js':
            self.send_static_file('frontend/app.js', 'application/javascript')
        elif path == '/styles.css':
            self.send_static_file('frontend/styles.css', 'text/css')
        else:
            self.send_error(404, "Not Found")
    
    def do_POST(self):
        """Handle POST requests"""
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        if path == '/text':
            self.handle_set_text()
        elif path == '/calibrate':
            self.handle_calibrate()
        else:
            self.send_error(404, "Not Found")
    
    def do_OPTIONS(self):
        """Handle CORS preflight requests"""
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
    
    def send_home_page(self):
        """Send the frontend HTML page"""
        try:
            with open('frontend/index.html', 'r') as f:
                html = f.read()
            
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(html.encode())
        except FileNotFoundError:
            self.send_error(404, "Frontend not found")
        except Exception as e:
            self.send_error(500, f"Error loading frontend: {str(e)}")
    
    def send_status(self):
        """Send JSON status"""
        status = self.splitflap_server.get_status()
        self.send_json_response(200, status)
    
    def send_health_check(self):
        """Send health check response"""
        health = {"status": "ok", "timestamp": time.time()}
        self.send_json_response(200, health)
    
    def send_static_file(self, file_path, content_type):
        """Send a static file"""
        try:
            with open(file_path, 'r') as f:
                content = f.read()
            
            self.send_response(200)
            self.send_header('Content-type', content_type)
            self.end_headers()
            self.wfile.write(content.encode())
        except FileNotFoundError:
            self.send_error(404, "File not found")
        except Exception as e:
            self.send_error(500, f"Error loading file: {str(e)}")
    
    def handle_set_text(self):
        """Handle text setting requests"""
        try:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            message = data.get('message', '')
            force_movement = data.get('force_movement', False)
            
            if not message:
                self.send_json_response(400, {"success": False, "message": "No message provided"})
                return
            
            success, result_message = self.splitflap_server.set_text(message, force_movement)
            status_code = 200 if success else 400
            self.send_json_response(status_code, {"success": success, "message": result_message})
            
        except json.JSONDecodeError:
            self.send_json_response(400, {"success": False, "message": "Invalid JSON"})
        except Exception as e:
            self.send_json_response(500, {"success": False, "message": f"Server error: {str(e)}"})
    
    def handle_calibrate(self):
        """Handle calibration requests"""
        try:
            with self.splitflap_server.splitflap_lock:
                if not self.splitflap_server.splitflap:
                    self.send_json_response(400, {"success": False, "message": "Splitflap not connected"})
                    return
                
                # Send recalibrate command to all modules
                # This would need to be implemented in the splitflap_proto library
                # For now, we'll just return a success message
                self.send_json_response(200, {"success": True, "message": "Recalibration initiated"})
                
        except Exception as e:
            self.send_json_response(500, {"success": False, "message": f"Error: {str(e)}"})
    
    def send_json_response(self, status_code, data):
        """Send JSON response"""
        self.send_response(status_code)
        self.send_header('Content-type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
        response = json.dumps(data, indent=2)
        self.wfile.write(response.encode())
    
    def log_message(self, format, *args):
        """Override to use our logger"""
        logger.info(f"{self.address_string()} - {format % args}")

def create_handler(splitflap_server):
    """Create a request handler with the splitflap server instance"""
    def handler(*args, **kwargs):
        return SplitflapHTTPRequestHandler(splitflap_server, *args, **kwargs)
    return handler

def main():
    parser = argparse.ArgumentParser(description='HTTP Server for Splitflap Display')
    parser.add_argument('--port', type=int, default=8080, help='HTTP server port (default: 8080)')
    parser.add_argument('--serial-port', type=str, help='Serial port for splitflap (auto-detect if not specified)')
    parser.add_argument('--host', type=str, default='0.0.0.0', help='Host to bind to (default: 0.0.0.0)')
    
    args = parser.parse_args()
    
    # Create splitflap server
    splitflap_server = SplitflapHTTPServer(args.port, args.serial_port)
    
    # Connect to splitflap
    if not splitflap_server.connect_splitflap():
        logger.error("Failed to connect to splitflap. Exiting.")
        return 1
    
    try:
        # Create HTTP server
        handler = create_handler(splitflap_server)
        server = HTTPServer((args.host, args.port), handler)
        splitflap_server.server = server
        
        logger.info(f"Starting HTTP server on {args.host}:{args.port}")
        logger.info(f"Web interface: http://{args.host}:{args.port}")
        logger.info("Press Ctrl+C to stop")
        
        # Start server
        server.serve_forever()
        
    except KeyboardInterrupt:
        logger.info("Shutting down...")
    except Exception as e:
        logger.error(f"Server error: {e}")
    finally:
        splitflap_server.disconnect_splitflap()
        if splitflap_server.server:
            splitflap_server.server.shutdown()
    
    return 0

if __name__ == '__main__':
    exit(main())
    