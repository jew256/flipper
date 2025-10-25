# ESP32 Web Server for Splitflap Display

This ESP32 firmware includes a built-in web server that hosts the splitflap display control interface directly on the ESP32. No separate Python server is needed!

## Features

- **Built-in Web Interface**: The complete frontend is embedded in the ESP32 firmware
- **REST API**: Provides endpoints for status, text control, and calibration
- **Wordle-style UI**: Clean, modern interface with brown background
- **Real-time Status**: Shows connection status and module count
- **Alphabet Keyboard**: Clickable keyboard showing available characters
- **Force Movement**: Option to force full rotation even for same characters

## Setup

### 1. Configure WiFi

Edit `splitflap/firmware/esp32/splitflap/secrets.h` and add your WiFi credentials:

```cpp
#define WIFI_SSID "your_wifi_network_name"
#define WIFI_PASSWORD "your_wifi_password"
```

### 2. Build and Upload

```bash
cd splitflap
pio run -e chainlink -t upload
```

### 3. Find the ESP32 IP Address

After uploading, check the serial monitor to see the ESP32's IP address:

```
WiFi connected. IP address: 192.168.1.100
Web server started on http://192.168.1.100
```

### 4. Access the Web Interface

Open your web browser and navigate to the IP address shown in the serial monitor:

```
http://192.168.1.100
```

## API Endpoints

The ESP32 web server provides these REST API endpoints:

### GET /
- **Description**: Serves the main web interface
- **Response**: HTML page with embedded CSS and JavaScript

### GET /status
- **Description**: Get current display status
- **Response**: JSON with connection status, module count, and alphabet
```json
{
  "connected": true,
  "num_modules": 6,
  "alphabet": [" ", "A", "B", "C", ...],
  "mode": "run"
}
```

### POST /text
- **Description**: Send text to display
- **Body**: JSON with message and optional force_movement flag
```json
{
  "message": "HELLO",
  "force_movement": false
}
```
- **Response**: JSON with success status
```json
{
  "success": true,
  "message": "Text sent to display"
}
```

### POST /calibrate
- **Description**: Trigger recalibration of all modules
- **Response**: JSON with success status
```json
{
  "success": true,
  "message": "Recalibration started"
}
```

## Configuration

The web server is enabled by default in the `chainlink` environment. To disable it, edit `platformio.ini`:

```ini
; Set to true to enable web server support (hosts frontend directly on ESP32)
-DWEB_SERVER=false
```

## Troubleshooting

### ESP32 Won't Connect to WiFi
- Check your WiFi credentials in `secrets.h`
- Ensure your WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Check the serial monitor for connection errors

### Web Interface Not Loading
- Verify the ESP32 IP address from the serial monitor
- Try accessing the IP address directly in your browser
- Check that your computer is on the same network as the ESP32

### Display Not Responding
- Check the serial monitor for any error messages
- Verify the splitflap display is properly connected
- Try the `/calibrate` endpoint to reset the modules

## Advantages Over Python Server

1. **No External Dependencies**: Everything runs on the ESP32
2. **Always Available**: No need to start a separate server
3. **Lower Latency**: Direct communication with the display
4. **Simpler Setup**: Just flash the firmware and connect to WiFi
5. **Portable**: Works anywhere with WiFi access

## Memory Usage

The embedded web interface adds approximately 50KB to the firmware size. The ESP32 has plenty of flash memory to accommodate this.

## Security Note

The web server is designed for local network use only. It doesn't include authentication or encryption. For production use, consider adding security measures.






