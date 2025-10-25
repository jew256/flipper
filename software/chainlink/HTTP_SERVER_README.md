# Splitflap HTTP Server

This HTTP server allows you to send text messages to your splitflap display over the network using HTTP requests.

## Features

- **Web Interface**: Simple HTML interface for easy control
- **REST API**: JSON API for programmatic control
- **Real-time Status**: Check connection status and display info
- **Character Validation**: Automatically validates characters against available alphabet
- **Thread-safe**: Safe concurrent access to the splitflap display

## Quick Start

1. **Start the server:**
   ```bash
   cd splitflap/software/chainlink
   python3 splitflap_http_server.py
   ```

2. **Open your browser:**
   ```
   http://localhost:8080
   ```

3. **Send messages via the web interface or API**

## Command Line Options

```bash
python3 splitflap_http_server.py [options]

Options:
  --port PORT          HTTP server port (default: 8080)
  --serial-port PORT   Serial port for splitflap (auto-detect if not specified)
  --host HOST          Host to bind to (default: 0.0.0.0)
```

## API Endpoints

### GET /
Returns the web interface HTML page.

### GET /status
Get current status of the splitflap display.

**Response:**
```json
{
  "connected": true,
  "num_modules": 6,
  "alphabet": [" ", "A", "B", "C", ...],
  "serial_port": "/dev/ttyUSB0"
}
```

### GET /health
Health check endpoint.

**Response:**
```json
{
  "status": "ok",
  "timestamp": 1234567890.123
}
```

### POST /text
Set text on the splitflap display.

**Request Body:**
```json
{
  "message": "hello",
  "force_movement": false
}
```

**Response:**
```json
{
  "success": true,
  "message": "Text set to: 'hello'"
}
```

### POST /calibrate
Recalibrate all modules (placeholder - needs implementation).

## Usage Examples

### Using curl

```bash
# Send a text message
curl -X POST http://localhost:8080/text \
  -H "Content-Type: application/json" \
  -d '{"message": "hello"}'

# Send with force movement
curl -X POST http://localhost:8080/text \
  -H "Content-Type: application/json" \
  -d '{"message": "hello", "force_movement": true}'

# Get status
curl http://localhost:8080/status
```

### Using Python requests

```python
import requests

# Send text
response = requests.post('http://localhost:8080/text', 
                        json={'message': 'hello'})
print(response.json())

# Get status
response = requests.get('http://localhost:8080/status')
print(response.json())
```

### Using the test client

```bash
# Run full test suite
python3 test_http_client.py

# Send a single message
python3 test_http_client.py --message "hello"

# Test remote server
python3 test_http_client.py --host 192.168.1.100 --port 8080
```

## Network Access

By default, the server binds to `0.0.0.0:8080`, making it accessible from other devices on your network.

**To access from another device:**
1. Find your computer's IP address: `ip addr show` or `ifconfig`
2. Open browser on another device: `http://YOUR_IP:8080`

**Security Note:** This server has no authentication. Only run on trusted networks.

## Troubleshooting

### "Splitflap not connected"
- Make sure your splitflap display is connected via USB
- Check that the serial port is correct
- Try power cycling the display

### "Invalid characters" error
- Check the available alphabet in the status endpoint
- Only use characters that are physically available on your flaps

### Connection refused
- Make sure the server is running
- Check the port number
- Verify firewall settings

## Dependencies

The server requires:
- `requests` (for the test client)
- Standard Python libraries (http.server, json, threading, etc.)
- The splitflap_proto library (included in this directory)

Install with:
```bash
pip install requests
```

## Integration Examples

### Home Assistant
You can integrate this with Home Assistant using RESTful commands:

```yaml
# configuration.yaml
rest_command:
  splitflap_text:
    url: "http://localhost:8080/text"
    method: POST
    headers:
      Content-Type: "application/json"
    payload: '{"message": "{{ message }}"}'
```

### Node-RED
Use an HTTP Request node to send messages:

- URL: `http://localhost:8080/text`
- Method: POST
- Headers: `Content-Type: application/json`
- Body: `{"message": "hello"}`

### IFTTT
Use IFTTT's Webhooks service to trigger messages from various events.






