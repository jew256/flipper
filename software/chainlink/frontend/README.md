# Splitflap Display Frontend

A beautiful, Wordle-style frontend for controlling your splitflap display.

## Features

- **Wordle-style Interface**: Clean, modern design with individual character boxes
- **Alphabet Keyboard**: Click on letters below to input characters
- **Keyboard Navigation**: Use arrow keys, backspace, and enter for quick input
- **Real-time Status**: Shows connection status and module count
- **Quick Actions**: Pre-defined buttons for common messages
- **Force Movement**: Option to force all modules to move (useful for calibration)
- **Responsive Design**: Works on desktop and mobile devices

## How to Use

1. **Start the HTTP server** (from the parent directory):
   ```bash
   python3 splitflap_http_server.py
   ```

2. **Open your browser** to `http://localhost:8080`

3. **Input text** by:
   - Clicking on individual boxes and typing
   - Clicking on alphabet keys below
   - Using keyboard navigation (arrow keys, backspace, enter)

4. **Send to display** by clicking "Send to Display" or pressing Ctrl+Enter

## Keyboard Shortcuts

- **Arrow Keys**: Navigate between boxes
- **Backspace**: Clear current box or go to previous box
- **Enter**: Send current text to display
- **Ctrl+Enter**: Send text to display
- **Ctrl+Backspace**: Clear all boxes

## Quick Actions

- **Hello**: Sets "hello"
- **Numbers**: Sets "12345"
- **Test**: Sets "test "
- **Clear**: Clears all boxes

## API Integration

The frontend communicates with the HTTP server via REST API:

- `GET /status` - Get display status and configuration
- `POST /text` - Send text message to display
- `GET /health` - Health check

## Files

- `index.html` - Main HTML structure
- `styles.css` - Wordle-style CSS with responsive design
- `app.js` - TypeScript/JavaScript application logic
- `app.ts` - TypeScript source (compiled to app.js)

## Customization

You can customize the appearance by modifying `styles.css`:

- Change colors by updating CSS variables
- Modify box sizes and spacing
- Adjust responsive breakpoints
- Customize the alphabet keyboard layout

## Browser Compatibility

- Modern browsers with ES2020 support
- Chrome, Firefox, Safari, Edge
- Mobile browsers (iOS Safari, Chrome Mobile)






