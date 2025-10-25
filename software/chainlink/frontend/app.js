// Configuration
const API_BASE_URL = window.location.origin;
const UPDATE_INTERVAL = 5000; // 5 seconds

// State
let currentStatus = null;
let currentText = [];
let currentFocusIndex = 0;
let numModules = 6; // Default, will be updated from server

// DOM Elements
const statusIndicator = document.getElementById('statusIndicator');
const statusText = document.getElementById('statusText');
const displayGrid = document.getElementById('displayGrid');
const alphabetKeyboard = document.getElementById('alphabetKeyboard');
const sendButton = document.getElementById('sendButton');
const clearButton = document.getElementById('clearButton');
const forceMovementButton = document.getElementById('forceMovementButton');
const toast = document.getElementById('toast');

// API Functions
async function fetchStatus() {
    try {
        const response = await fetch(`${API_BASE_URL}/status`);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return await response.json();
    } catch (error) {
        console.error('Error fetching status:', error);
        throw error;
    }
}

async function sendText(text, forceMovement = false) {
    try {
        const response = await fetch(`${API_BASE_URL}/text`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                message: text,
                force_movement: forceMovement
            })
        });
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        return await response.json();
    } catch (error) {
        console.error('Error sending text:', error);
        throw error;
    }
}

// UI Functions
function showToast(message, type = 'success') {
    toast.textContent = message;
    toast.className = `toast ${type}`;
    toast.classList.add('show');
    
    setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

function updateStatusDisplay(status) {
    currentStatus = status;
    
    if (status.connected) {
        statusIndicator.classList.add('connected');
        statusText.textContent = `Connected (${status.num_modules} modules)`;
        numModules = status.num_modules;
        sendButton.disabled = false;
        
        // Always create alphabet keyboard when connected
        createAlphabetKeyboard();
    } else {
        statusIndicator.classList.remove('connected');
        statusText.textContent = `Disconnected: ${status.error || 'Unknown error'}`;
        sendButton.disabled = true;
    }
    
    // Update display grid if number of modules changed
    if (status.connected && status.num_modules !== currentText.length) {
        initializeDisplay();
    }
}

function initializeDisplay() {
    // Clear existing boxes
    displayGrid.innerHTML = '';
    
    // Create boxes for each module
    currentText = new Array(numModules).fill(' ');
    
    for (let i = 0; i < numModules; i++) {
        const box = document.createElement('div');
        box.className = 'display-box';
        box.textContent = ' ';
        box.dataset.index = i.toString();
        box.tabIndex = 0; // Make focusable
        
        // Add click handler
        box.addEventListener('click', () => focusBox(i));
        
        // Add keyboard input handler
        box.addEventListener('keydown', (e) => handleBoxKeydown(e, i));
        
        displayGrid.appendChild(box);
    }
    
    // Focus first box
    focusBox(0);
}

function focusBox(index) {
    // Remove focus from all boxes
    document.querySelectorAll('.display-box').forEach(box => {
        box.classList.remove('focused');
    });
    
    // Add focus to selected box
    const box = document.querySelector(`[data-index="${index}"]`);
    if (box) {
        box.classList.add('focused');
        box.focus();
        currentFocusIndex = index;
    }
}

function updateDisplayBox(index, char) {
    const box = document.querySelector(`[data-index="${index}"]`);
    if (box) {
        box.textContent = char;
        box.classList.toggle('filled', char !== ' ');
    }
}

function handleBoxKeydown(event, index) {
    event.preventDefault();
    
    const key = event.key.toLowerCase();
    
    // Handle special keys
    if (key === 'backspace') {
        if (currentText[index] !== ' ') {
            currentText[index] = ' ';
            updateDisplayBox(index, ' ');
        } else if (index > 0) {
            focusBox(index - 1);
        }
        return;
    }
    
    if (key === 'arrowleft' && index > 0) {
        focusBox(index - 1);
        return;
    }
    
    if (key === 'arrowright' && index < numModules - 1) {
        focusBox(index + 1);
        return;
    }
    
    if (key === 'enter') {
        sendCurrentText();
        return;
    }
    
    // Handle character input
    if (currentStatus?.alphabet.includes(key.toUpperCase())) {
        currentText[index] = key.toUpperCase();
        updateDisplayBox(index, key.toUpperCase());
        
        // Move to next box if not the last one
        if (index < numModules - 1) {
            focusBox(index + 1);
        }
    }
}

function createAlphabetKeyboard() {
    // Use server alphabet if available, otherwise use default
    const alphabet = currentStatus?.alphabet || [' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', "'"];
    
    console.log('Creating alphabet keyboard with:', alphabet);
    alphabetKeyboard.innerHTML = '';
    
    alphabet.forEach(char => {
        const key = document.createElement('div');
        key.className = 'key';
        key.textContent = char;
        
        if (char === ' ') {
            key.classList.add('space');
            key.textContent = 'SPACE';
        }
        
        key.addEventListener('click', () => {
            console.log('Clicked key:', char);
            if (currentText[currentFocusIndex] !== ' ') {
                key.classList.add('used');
            }
            
            currentText[currentFocusIndex] = char;
            updateDisplayBox(currentFocusIndex, char);
            
            // Move to next box if not the last one
            if (currentFocusIndex < numModules - 1) {
                focusBox(currentFocusIndex + 1);
            }
        });
        
        alphabetKeyboard.appendChild(key);
    });
    
    console.log('Alphabet keyboard created with', alphabet.length, 'keys');
}

function sendCurrentText() {
    if (!currentStatus?.connected) {
        showToast('Not connected to splitflap display', 'error');
        return;
    }
    
    const text = currentText.join('');
    sendText(text)
        .then(response => {
            if (response.success) {
                showToast(`Sent "${text}"`, 'success');
            } else {
                showToast(response.message, 'error');
            }
        })
        .catch(error => {
            showToast(`Error: ${error.message}`, 'error');
        });
}

function sendCurrentTextWithForceMovement() {
    if (!currentStatus?.connected) {
        showToast('Not connected to splitflap display', 'error');
        return;
    }
    
    const text = currentText.join('');
    sendText(text, true)
        .then(response => {
            if (response.success) {
                showToast(`Sent "${text}" (force movement)`, 'success');
            } else {
                showToast(response.message, 'error');
            }
        })
        .catch(error => {
            showToast(`Error: ${error.message}`, 'error');
        });
}

function clearDisplay() {
    currentText = new Array(numModules).fill(' ');
    
    for (let i = 0; i < numModules; i++) {
        updateDisplayBox(i, ' ');
    }
    
    focusBox(0);
    
    // Reset keyboard key states
    document.querySelectorAll('.key').forEach(key => {
        key.classList.remove('used');
    });
}



// Event Listeners
sendButton.addEventListener('click', sendCurrentText);
clearButton.addEventListener('click', clearDisplay);
forceMovementButton.addEventListener('click', sendCurrentTextWithForceMovement);



// Global keyboard shortcuts
document.addEventListener('keydown', (e) => {
    if (e.ctrlKey || e.metaKey) {
        switch (e.key) {
            case 'Enter':
                e.preventDefault();
                sendCurrentText();
                break;
            case 'Backspace':
                e.preventDefault();
                clearDisplay();
                break;
        }
    }
});

// Status update loop
async function updateStatus() {
    try {
        console.log('Fetching status...');
        const status = await fetchStatus();
        console.log('Status received:', status);
        updateStatusDisplay(status);
    } catch (error) {
        console.error('Failed to fetch status:', error);
        statusText.textContent = 'Connection error';
        statusIndicator.classList.remove('connected');
        sendButton.disabled = true;
    }
}

// Initialize app
async function init() {
    console.log('Initializing app...');
    
    // Initialize display first
    initializeDisplay();
    
    // Create alphabet keyboard with default alphabet
    createAlphabetKeyboard();
    
    // Initial status check
    await updateStatus();
    
    // Set up periodic status updates
    setInterval(updateStatus, UPDATE_INTERVAL);
    
    console.log('App initialized');
}

// Start the app
document.addEventListener('DOMContentLoaded', init);