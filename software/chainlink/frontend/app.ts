// Types
interface SplitflapStatus {
    connected: boolean;
    num_modules: number;
    alphabet: string[];
    serial_port?: string;
    error?: string;
}

interface ApiResponse {
    success: boolean;
    message: string;
}

// Configuration
const API_BASE_URL = window.location.origin;
const UPDATE_INTERVAL = 5000; // 5 seconds

// State
let currentStatus: SplitflapStatus | null = null;
let currentText: string[] = [];
let currentFocusIndex = 0;
let numModules = 6; // Default, will be updated from server

// DOM Elements
const statusIndicator = document.getElementById('statusIndicator') as HTMLElement;
const statusText = document.getElementById('statusText') as HTMLElement;
const displayGrid = document.getElementById('displayGrid') as HTMLElement;
const alphabetKeyboard = document.getElementById('alphabetKeyboard') as HTMLElement;
const sendButton = document.getElementById('sendButton') as HTMLButtonElement;
const clearButton = document.getElementById('clearButton') as HTMLButtonElement;
const forceMovementButton = document.getElementById('forceMovementButton') as HTMLButtonElement;
const toast = document.getElementById('toast') as HTMLElement;

// API Functions
async function fetchStatus(): Promise<SplitflapStatus> {
    const response = await fetch(`${API_BASE_URL}/status`);
    if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
    }
    return await response.json();
}

async function sendText(text: string, forceMovement = false): Promise<ApiResponse> {
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
}

// UI Functions
function showToast(message: string, type: 'success' | 'error' = 'success') {
    toast.textContent = message;
    toast.className = `toast ${type}`;
    toast.classList.add('show');
    
    setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

function updateStatusDisplay(status: SplitflapStatus) {
    currentStatus = status;
    
    if (status.connected) {
        statusIndicator.classList.add('connected');
        statusText.textContent = `Connected (${status.num_modules} modules)`;
        numModules = status.num_modules;
        sendButton.disabled = false;
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
        
        // Add click handler
        box.addEventListener('click', () => focusBox(i));
        
        // Add keyboard input handler
        box.addEventListener('keydown', (e) => handleBoxKeydown(e, i));
        
        displayGrid.appendChild(box);
    }
    
    // Focus first box
    focusBox(0);
}

function focusBox(index: number) {
    // Remove focus from all boxes
    document.querySelectorAll('.display-box').forEach(box => {
        box.classList.remove('focused');
    });
    
    // Add focus to selected box
    const box = document.querySelector(`[data-index="${index}"]`) as HTMLElement;
    if (box) {
        box.classList.add('focused');
        box.focus();
        currentFocusIndex = index;
    }
}

function updateDisplayBox(index: number, char: string) {
    const box = document.querySelector(`[data-index="${index}"]`) as HTMLElement;
    if (box) {
        box.textContent = char;
        box.classList.toggle('filled', char !== ' ');
    }
}

function handleBoxKeydown(event: KeyboardEvent, index: number) {
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
    if (!currentStatus?.alphabet) return;
    
    alphabetKeyboard.innerHTML = '';
    
    currentStatus.alphabet.forEach(char => {
        const key = document.createElement('div');
        key.className = 'key';
        key.textContent = char;
        
        if (char === ' ') {
            key.classList.add('space');
            key.textContent = 'SPACE';
        }
        
        key.addEventListener('click', () => {
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
                showToast(response.message, 'success');
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
                showToast(response.message, 'success');
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

function setQuickText(text: string) {
    const chars = text.split('');
    
    for (let i = 0; i < numModules; i++) {
        const char = i < chars.length ? chars[i] : ' ';
        currentText[i] = char;
        updateDisplayBox(i, char);
    }
    
    focusBox(0);
}

// Event Listeners
sendButton.addEventListener('click', sendCurrentText);
clearButton.addEventListener('click', clearDisplay);
forceMovementButton.addEventListener('click', sendCurrentTextWithForceMovement);

// Quick action buttons
document.addEventListener('click', (e) => {
    const target = e.target as HTMLElement;
    if (target.classList.contains('btn-quick') && target.dataset.text) {
        setQuickText(target.dataset.text);
    }
});

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
        const status = await fetchStatus();
        updateStatusDisplay(status);
        
        if (status.connected && !alphabetKeyboard.hasChildNodes()) {
            createAlphabetKeyboard();
        }
    } catch (error) {
        console.error('Failed to fetch status:', error);
        statusText.textContent = 'Connection error';
        statusIndicator.classList.remove('connected');
        sendButton.disabled = true;
    }
}

// Initialize app
async function init() {
    // Initial status check
    await updateStatus();
    
    // Set up periodic status updates
    setInterval(updateStatus, UPDATE_INTERVAL);
    
    // Initialize display
    initializeDisplay();
}

// Start the app
document.addEventListener('DOMContentLoaded', init);






