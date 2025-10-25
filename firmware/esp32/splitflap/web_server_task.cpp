/*
   Copyright 2024 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "web_server_task.h"
#include "secrets.h"
#include "config.h"

// Embedded HTML content
const char WebServerTask::HTML_CONTENT[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Splitflap Display Controller</title>
    <style>
/* Reset and base styles */
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: #6aaa64;
    min-height: 100vh;
    color: #333;
    position: relative;
}

.container {
    max-width: 800px;
    margin: 0 auto;
    padding: 20px;
}

/* Header */
header {
    text-align: center;
    margin-bottom: 40px;
}

h1 {
    color: white;
    font-size: 2.5rem;
    font-weight: 700;
    margin-bottom: 10px;
    text-shadow: 0 2px 4px rgba(0,0,0,0.3);
    letter-spacing: 2px;
}

.status {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 10px;
    color: white;
    font-size: 1.1rem;
}

.status-indicator {
    width: 12px;
    height: 12px;
    border-radius: 50%;
    background: #ff6b6b;
    animation: pulse 2s infinite;
}

.status-indicator.connected {
    background: #51cf66;
}

@keyframes pulse {
    0% { opacity: 1; }
    50% { opacity: 0.5; }
    100% { opacity: 1; }
}

/* History Sidebar */
.history-sidebar {
    position: fixed;
    top: 0;
    right: 0;
    width: 300px;
    height: 100vh;
    background: #faf8f5;
    box-shadow: -2px 0 10px rgba(0,0,0,0.1);
    transition: right 0.3s ease;
    z-index: 1000;
    overflow-y: auto;
}

.history-sidebar.closed {
    right: -300px;
}

.history-header {
    padding: 20px;
    background: #8B4513;
    color: white;
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.header-buttons {
    display: flex;
    gap: 10px;
    align-items: center;
}

.history-title {
    font-size: 1.2rem;
    font-weight: 600;
}

.history-close {
    background: none;
    border: none;
    color: white;
    font-size: 1.5rem;
    cursor: pointer;
    padding: 0;
    width: 30px;
    height: 30px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 50%;
    transition: background 0.2s ease;
}

.history-close:hover {
    background: rgba(255,255,255,0.2);
}

.logout-btn {
    background: none;
    border: none;
    color: white;
    font-size: 1.2rem;
    cursor: pointer;
    padding: 5px;
    width: 30px;
    height: 30px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 50%;
    transition: background 0.2s ease;
}

.logout-btn:hover {
    background: rgba(255,255,255,0.2);
}

.history-list {
    padding: 0 20px 20px 20px;
}

.current-state-section, .admin-login-section, .admin-section, .history-section {
    padding: 20px;
    border-bottom: 1px solid #e0e0e0;
}

.history-section:last-child {
    border-bottom: none;
}

.admin-login {
    display: flex;
    flex-direction: column;
    gap: 10px;
    align-items: center;
}

.admin-login-btn {
    padding: 10px 20px;
    border: 2px solid #8B4513;
    border-radius: 6px;
    background: #8B4513;
    color: white;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s ease;
    font-size: 0.9rem;
    width: 100%;
}

.admin-login-btn:hover {
    background: #6B3410;
    border-color: #6B3410;
}

.admin-status {
    font-size: 0.8rem;
    color: #666;
    text-align: center;
    font-style: italic;
}

.admin-logout-btn {
    background: #dc3545;
    border-color: #dc3545;
    color: white;
}

.admin-logout-btn:hover {
    background: #c82333;
    border-color: #c82333;
}

/* Disabled state overlay */
.disabled-overlay {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(0, 0, 0, 0.7);
    z-index: 1000;
    display: none;
    justify-content: center;
    align-items: center;
    flex-direction: column;
    color: white;
    font-size: 1.2rem;
    text-align: center;
}

.disabled-overlay.show {
    display: flex;
}

.disabled-icon {
    font-size: 4rem;
    margin-bottom: 20px;
    opacity: 0.8;
}

.disabled-message {
    background: rgba(0, 0, 0, 0.8);
    padding: 20px 30px;
    border-radius: 10px;
    border: 2px solid #ff6b6b;
    max-width: 400px;
}

.disabled-title {
    font-size: 1.5rem;
    font-weight: bold;
    margin-bottom: 10px;
    color: #ff6b6b;
}

.disabled-subtitle {
    font-size: 1rem;
    opacity: 0.9;
    line-height: 1.4;
}

.admin-controls {
    display: flex;
    flex-direction: column;
    gap: 10px;
}

.admin-btn {
    padding: 10px 15px;
    border: 2px solid #8B4513;
    border-radius: 6px;
    background: white;
    color: #8B4513;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s ease;
    font-size: 0.9rem;
}

.admin-btn:hover {
    background: #8B4513;
    color: white;
}

.admin-btn:disabled {
    opacity: 0.5;
    cursor: not-allowed;
}

/* Admin-only sections */
.admin-only {
    display: none;
}

.admin-only.show {
    display: block;
}

.section-title {
    font-size: 1rem;
    font-weight: 600;
    color: #8B4513;
    margin-bottom: 15px;
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

.current-display-item {
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 15px 0;
    text-align: center;
}

.state-label {
    font-weight: 500;
    color: #666;
    margin-bottom: 8px;
    font-size: 0.9rem;
}

.current-word {
    font-weight: 700;
    color: #8B4513;
    font-family: monospace;
    font-size: 1.4rem;
    letter-spacing: 2px;
    background: #f8f9fa;
    padding: 10px 15px;
    border-radius: 8px;
    border: 2px solid #8B4513;
    min-width: 120px;
}

.history-item {
    padding: 12px;
    margin-bottom: 8px;
    background: #f8f9fa;
    border-radius: 8px;
    border-left: 4px solid #8B4513;
    font-family: monospace;
    font-size: 1.1rem;
    word-break: break-all;
}

.history-item:last-child {
    margin-bottom: 0;
}

.history-toggle {
    position: fixed;
    top: 20px;
    right: 20px;
    background: white;
    border: none;
    border-radius: 50%;
    width: 50px;
    height: 50px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 1.2rem;
    color: #8B4513;
    transition: all 0.2s ease;
    z-index: 1001;
}

.history-toggle:hover {
    transform: scale(1.1);
    box-shadow: 0 4px 15px rgba(0,0,0,0.2);
}

.history-toggle::before {
    content: "→";
}

.history-sidebar.closed + .history-toggle::before {
    content: "📋";
}

/* Main content */
main {
    background: white;
    border-radius: 12px;
    padding: 40px;
    box-shadow: 0 4px 6px rgba(0,0,0,0.1);
    border: 1px solid #d3d6da;
}

/* Display Grid - Wordle style */
.display-grid {
    display: flex;
    justify-content: center;
    gap: 8px;
    margin-bottom: 40px;
}

.display-box {
    width: 60px;
    height: 60px;
    border: 3px solid #d3d6da;
    border-radius: 0;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 2rem;
    font-weight: bold;
    background: white;
    transition: all 0.2s ease;
    cursor: pointer;
    position: relative;
    text-transform: uppercase;
}

.display-box:hover {
    border-color: #878a8c;
    transform: scale(1.05);
}

.display-box.focused {
    border-color: #878a8c;
    box-shadow: 0 0 0 2px rgba(135, 138, 140, 0.3);
}

.display-box.filled {
    border-color: #878a8c;
    background: white;
}

/* Controls */
.controls {
    display: flex;
    justify-content: center;
    gap: 15px;
    margin-bottom: 40px;
    flex-wrap: wrap;
}

.btn {
    padding: 12px 24px;
    border: none;
    border-radius: 8px;
    font-size: 1rem;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s ease;
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

.btn:disabled {
    opacity: 0.5;
    cursor: not-allowed;
}

.btn-primary {
    background: #6aaa64;
    color: white;
}

.btn-primary:hover:not(:disabled) {
    background: #5a9a54;
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(106, 170, 100, 0.3);
}

.btn-secondary {
    background: #d3d6da;
    color: #333;
}

.btn-secondary:hover {
    background: #c4c7ca;
    transform: translateY(-2px);
}

.btn-warning {
    background: #f7b731;
    color: white;
}

.btn-warning:hover {
    background: #e6a429;
    transform: translateY(-2px);
}

/* Alphabet Keyboard */
.alphabet-keyboard {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 6px;
    margin-bottom: 40px;
    padding: 20px;
    background: transparent;
}

.key {
    width: 40px;
    height: 40px;
    border: 2px solid #d3d6da;
    border-radius: 4px;
    background: #d3d6da;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 1.1rem;
    font-weight: bold;
    cursor: pointer;
    transition: all 0.2s ease;
    user-select: none;
    color: #333;
    text-transform: uppercase;
}

.key:hover {
    background: #878a8c;
    border-color: #878a8c;
    transform: scale(1.05);
}

.key:active {
    transform: scale(0.95);
}

.key.space {
    width: 80px;
    font-size: 0.8rem;
    color: #666;
}

.key.used {
    background: #6aaa64;
    color: white;
    border-color: #6aaa64;
}

/* Toast Notifications */
.toast {
    position: fixed;
    top: 20px;
    left: 20px;
    background: #333;
    color: white;
    padding: 15px 20px;
    border-radius: 8px;
    box-shadow: 0 4px 12px rgba(0,0,0,0.3);
    transform: translateX(-400px);
    transition: transform 0.3s ease;
    z-index: 1000;
}

.toast.show {
    transform: translateX(0);
}

.toast.success {
    background: #f7b731;
}

.toast.error {
    background: #ff6b6b;
}

/* Responsive Design */
@media (max-width: 768px) {
    .container {
        padding: 10px;
    }
    
    main {
        padding: 20px;
    }
    
    h1 {
        font-size: 2rem;
    }
    
    .display-box {
        width: 50px;
        height: 50px;
        font-size: 1.5rem;
    }
    
    .key {
        width: 35px;
        height: 35px;
        font-size: 1rem;
    }
    
    .key.space {
        width: 70px;
    }
    
    .controls {
        flex-direction: column;
        align-items: center;
    }
    
    .btn {
        width: 200px;
    }
}
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>flipper</h1>
            <div class="status" id="status">
                <span class="status-indicator" id="statusIndicator"></span>
                <span id="statusText">Connecting...</span>
            </div>
        </header>

        <main>
            <!-- Display Grid -->
            <div class="display-grid" id="displayGrid">
                <!-- Boxes will be generated by JavaScript -->
            </div>

            <!-- Controls -->
            <div class="controls">
                <button id="sendButton" class="btn btn-primary" disabled>
                    Send to Display
                </button>
                <button id="clearButton" class="btn btn-secondary">
                    Clear
                </button>
                <button id="forceMovementButton" class="btn btn-warning">
                    Force Movement
                </button>
            </div>

            <!-- Alphabet Keyboard -->
            <div class="alphabet-keyboard" id="alphabetKeyboard">
                <!-- Alphabet keys will be generated by JavaScript -->
            </div>
        </main>

        <!-- Toast Notifications -->
        <div id="toast" class="toast"></div>
    </div>

    <!-- History Sidebar -->
    <button class="history-toggle" id="historyToggle" title="Toggle History"></button>
    
    <div class="history-sidebar closed" id="historySidebar">
        <div class="history-header">
            <div class="history-title">Display Info</div>
            <button class="history-close" id="historyClose">×</button>
        </div>
        
        <!-- Current State Section -->
        <div class="current-state-section admin-only" id="currentStateSection">
            <div class="section-title">Current Display</div>
            <div class="current-display-item">
                <span class="state-label">Now Showing:</span>
                <span class="state-value current-word" id="currentWord">---</span>
            </div>
        </div>
        
        <!-- Admin Login Section -->
        <div class="admin-login-section" id="adminLoginSection">
            <div class="section-title">Admin Access</div>
            <div class="admin-login">
                <button class="admin-login-btn" id="adminLoginBtn">Login as Admin</button>
                <div class="admin-status" id="adminStatus">Guest Mode</div>
            </div>
        </div>
        
        <!-- Admin Controls Section -->
        <div class="admin-section" id="adminSection" style="display: none;">
            <div class="section-title">Admin Controls</div>
            <div class="admin-controls">
                <button class="admin-btn" id="enableBtn">Enable Display</button>
                <button class="admin-btn" id="disableBtn">Disable Display</button>
                <button class="admin-btn admin-logout-btn" id="adminLogoutBtn">Logout Admin</button>
            </div>
        </div>
        
        <!-- History Section -->
        <div class="history-section admin-only" id="historySection">
            <div class="section-title">Last 10 Words</div>
            <div class="history-list" id="historyList">
                <div class="history-item">Loading...</div>
            </div>
        </div>
    </div>

    <script>
// Configuration
const API_BASE_URL = window.location.origin;
const UPDATE_INTERVAL = 5000; // 5 seconds

// State
let currentStatus = null;
let currentText = [];
let currentFocusIndex = 0;
let numModules = 6; // Default, will be updated from server
let history = []; // Array to store last 10 words sent
let currentDisplayedWord = '---'; // Currently displayed word on splitflap
let isAdminMode = false; // Whether user is in admin mode
let displayEnabled = true; // Whether display is enabled

// DOM Elements
const statusIndicator = document.getElementById('statusIndicator');
const statusText = document.getElementById('statusText');
const displayGrid = document.getElementById('displayGrid');
const alphabetKeyboard = document.getElementById('alphabetKeyboard');
const sendButton = document.getElementById('sendButton');
const clearButton = document.getElementById('clearButton');
const forceMovementButton = document.getElementById('forceMovementButton');
const toast = document.getElementById('toast');
const historyToggle = document.getElementById('historyToggle');
const historySidebar = document.getElementById('historySidebar');
const historyClose = document.getElementById('historyClose');
const historyList = document.getElementById('historyList');
const currentWordEl = document.getElementById('currentWord');
const adminLoginSection = document.getElementById('adminLoginSection');
const adminLoginBtn = document.getElementById('adminLoginBtn');
const adminStatus = document.getElementById('adminStatus');
const adminSection = document.getElementById('adminSection');
const enableBtn = document.getElementById('enableBtn');
const disableBtn = document.getElementById('disableBtn');
const adminLogoutBtn = document.getElementById('adminLogoutBtn');
const disabledOverlay = document.getElementById('disabledOverlay');

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

async function fetchHistory() {
    try {
        const response = await fetch(`${API_BASE_URL}/history`);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return await response.json();
    } catch (error) {
        console.error('Error fetching history:', error);
        throw error;
    }
}

async function fetchAdminStatus() {
    try {
        const response = await fetch(`${API_BASE_URL}/admin/status`);
        if (!response.ok) {
            if (response.status === 401) {
                // User is not authenticated as admin - guest mode
                return { admin_mode: false, guest_mode: true };
            }
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return await response.json();
    } catch (error) {
        console.error('Error fetching admin status:', error);
        return { admin_mode: false, guest_mode: true };
    }
}

async function sendAdminAction(action) {
    try {
        const response = await fetch(`${API_BASE_URL}/admin/control`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ action: action })
        });
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        return await response.json();
    } catch (error) {
        console.error('Error sending admin action:', error);
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
            if (response.status === 403) {
                // Display is disabled
                const errorData = await response.json();
                throw new Error(errorData.message || 'Display is disabled by admin');
            }
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        return await response.json();
    } catch (error) {
        console.error('Error sending text:', error);
        throw error;
    }
}

// History Management Functions
async function loadHistory() {
    try {
        const response = await fetchHistory();
        history = response.history || [];
        updateHistoryDisplay();
    } catch (error) {
        console.error('Failed to load history:', error);
        historyList.innerHTML = '<div class="history-item">Failed to load history</div>';
    }
}

function updateHistoryDisplay() {
    if (history.length === 0) {
        historyList.innerHTML = '<div class="history-item">No history yet</div>';
        // Update current word display
        currentDisplayedWord = '---';
        currentWordEl.textContent = '---';
        return;
    }
    
    historyList.innerHTML = history.map(item => 
        `<div class="history-item">${escapeHtml(item)}</div>`
    ).join('');
    
    // Update current word display with the most recent history item
    currentDisplayedWord = history[0] || '---';
    currentWordEl.textContent = currentDisplayedWord;
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

function toggleHistorySidebar() {
    historySidebar.classList.toggle('closed');
}

function closeHistorySidebar() {
    historySidebar.classList.add('closed');
}


// Admin Functions
async function checkAdminMode() {
    try {
        const adminStatusResponse = await fetchAdminStatus();
        isAdminMode = adminStatusResponse.admin_mode || false;
        displayEnabled = adminStatusResponse.enabled !== false;
        
        // Update UI based on admin status
        if (isAdminMode) {
            adminLoginSection.style.display = 'none';
            adminSection.style.display = 'block';
            adminStatus.textContent = 'Admin Mode';
            
            // Show admin-only sections
            const currentStateSection = document.getElementById('currentStateSection');
            const historySection = document.getElementById('historySection');
            if (currentStateSection) currentStateSection.classList.add('show');
            if (historySection) historySection.classList.add('show');
            
            updateAdminButtons();
            
            // Update disabled overlay based on display state
            if (displayEnabled) {
                hideDisabledOverlay();
            } else {
                showDisabledOverlay();
            }
        } else {
            adminLoginSection.style.display = 'block';
            adminSection.style.display = 'none';
            adminStatus.textContent = 'Guest Mode';
            
            // Hide admin-only sections
            const currentStateSection = document.getElementById('currentStateSection');
            const historySection = document.getElementById('historySection');
            if (currentStateSection) currentStateSection.classList.remove('show');
            if (historySection) historySection.classList.remove('show');
        }
        
        return isAdminMode;
    } catch (error) {
        console.error('Failed to check admin mode:', error);
        isAdminMode = false;
        adminLoginSection.style.display = 'block';
        adminSection.style.display = 'none';
        adminStatus.textContent = 'Guest Mode';
        
        // Hide admin-only sections on error
        const currentStateSection = document.getElementById('currentStateSection');
        const historySection = document.getElementById('historySection');
        if (currentStateSection) currentStateSection.classList.remove('show');
        if (historySection) historySection.classList.remove('show');
        
        return false;
    }
}

function updateAdminButtons() {
    if (displayEnabled) {
        enableBtn.disabled = true;
        disableBtn.disabled = false;
    } else {
        enableBtn.disabled = false;
        disableBtn.disabled = true;
    }
}

async function handleAdminAction(action) {
    try {
        const response = await sendAdminAction(action);
        if (response.success) {
            displayEnabled = (action === 'enable');
            updateAdminButtons();
            
            // Update disabled overlay
            if (displayEnabled) {
                hideDisabledOverlay();
            } else {
                showDisabledOverlay();
            }
            
            showToast(response.message, 'success');
        } else {
            showToast(response.message, 'error');
        }
    } catch (error) {
        showToast(`Error: ${error.message}`, 'error');
    }
}

async function loginAsAdmin() {
    // Trigger admin authentication by trying to access admin endpoint
    try {
        const response = await fetch(`${API_BASE_URL}/admin/status`);
        if (response.ok) {
            // Successfully authenticated as admin
            await checkAdminMode();
            showToast('Logged in as admin', 'success');
        } else if (response.status === 401) {
            // Authentication required - browser will show login prompt
            showToast('Please enter admin credentials', 'error');
        } else {
            showToast('Login failed', 'error');
        }
    } catch (error) {
        showToast(`Login error: ${error.message}`, 'error');
    }
}

function logoutAdmin() {
    // Clear admin credentials and refresh
    window.location.href = `${API_BASE_URL}/logout`;
}

function showDisabledOverlay() {
    if (disabledOverlay) {
        disabledOverlay.classList.add('show');
    }
}

function hideDisabledOverlay() {
    if (disabledOverlay) {
        disabledOverlay.classList.remove('show');
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
        
        // Update history and current word from status
        if (status.history) {
            history = status.history;
            updateHistoryDisplay();
        }
        
        if (status.current_word) {
            currentDisplayedWord = status.current_word;
            currentWordEl.textContent = status.current_word;
        }
        
        // Handle display enabled/disabled state
        if (status.display_enabled === false) {
            showDisabledOverlay();
        } else {
            hideDisabledOverlay();
        }
    } else {
        statusIndicator.classList.remove('connected');
        statusText.textContent = `Disconnected: ${status.error || 'Unknown error'}`;
        sendButton.disabled = true;
        
        // Update current word display
        currentWordEl.textContent = '---';
        hideDisabledOverlay();
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
                // Reload history from device (this will update current word display)
                loadHistory();
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
                // Reload history from device (this will update current word display)
                loadHistory();
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

// History sidebar event listeners
historyToggle.addEventListener('click', toggleHistorySidebar);
historyClose.addEventListener('click', closeHistorySidebar);

// Admin control event listeners
enableBtn.addEventListener('click', () => handleAdminAction('enable'));
disableBtn.addEventListener('click', () => handleAdminAction('disable'));
adminLoginBtn.addEventListener('click', loginAsAdmin);
adminLogoutBtn.addEventListener('click', logoutAdmin);

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
    
    // Initialize current word display
    currentWordEl.textContent = '---';
    
    // Load history from device
    await loadHistory();
    
    // Check admin mode
    await checkAdminMode();
    
    // Initial status check
    await updateStatus();
    
    // Set up periodic status updates
    setInterval(updateStatus, UPDATE_INTERVAL);
    
    console.log('App initialized');
}

// Start the app
document.addEventListener('DOMContentLoaded', init);
    </script>
    
    <!-- Disabled State Overlay -->
    <div class="disabled-overlay" id="disabledOverlay">
        <div class="disabled-icon">🚫</div>
        <div class="disabled-message">
            <div class="disabled-title">Display Disabled</div>
            <div class="disabled-subtitle">The display has been disabled by an administrator.<br>Please contact an admin to re-enable it.</div>
        </div>
    </div>
</body>
</html>
)rawliteral";

WebServerTask::WebServerTask(SplitflapTask& splitflap_task, Logger& logger, const uint8_t task_core, const uint16_t port)
    : Task("WebServer", 8192, 1, task_core), splitflap_task_(splitflap_task), logger_(logger), server_(port) {
}

void WebServerTask::run() {
    char buf[200];
    
    logger_.log("Starting web server task...");
    
    // Connect to WiFi if not already connected
    if (WiFi.status() != WL_CONNECTED) {
        logger_.log("Connecting to WiFi...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(1000);
            attempts++;
            snprintf(buf, sizeof(buf), "WiFi connection attempt %d", attempts);
            logger_.log(buf);
        }
        
        if (WiFi.status() != WL_CONNECTED) {
            logger_.log("Failed to connect to WiFi");
            return;
        }
    }
    
    snprintf(buf, sizeof(buf), "WiFi connected. IP address: %s", WiFi.localIP().toString().c_str());
    logger_.log(buf);
    
    setupWebServer();
    server_.begin();
    server_started_ = true;
    
    snprintf(buf, sizeof(buf), "Web server started on http://%s", WiFi.localIP().toString().c_str());
    logger_.log(buf);
    
    while (1) {
        server_.handleClient();
        delay(1);
    }
}

bool WebServerTask::checkAuthentication() {
    // Allow guest access - no authentication required for basic functionality
    return true;
}

bool WebServerTask::checkAdminAuthentication() {
    if (!server_.authenticate(WEB_SERVER_USERNAME, WEB_SERVER_PASSWORD)) {
        server_.requestAuthentication(BASIC_AUTH, "Splitflap Display Controller - Admin");
        return false;
    }
    return true;
}

void WebServerTask::setupWebServer() {
    // Root page - serve the embedded HTML
    server_.on("/", [this]() {
        if (!checkAuthentication()) return;
        server_.send(200, "text/html", HTML_CONTENT);
    });
    
    // API endpoints
    server_.on("/status", [this]() {
        if (!checkAuthentication()) return;
        String statusJson = getStatusJSON();
        server_.send(200, "application/json", statusJson);
    });
    
    server_.on("/text", HTTP_POST, [this]() {
        if (!checkAuthentication()) return;
        
        // Check if display is enabled
        if (!display_enabled_) {
            server_.send(403, "application/json", "{\"success\": false, \"message\": \"Display is disabled by admin\"}");
            return;
        }
        
        if (!server_.hasArg("plain")) {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"No JSON data provided\"}");
            return;
        }
        
        String jsonString = server_.arg("plain");
        
        // Parse JSON
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, jsonString);
        
        if (error) {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"Invalid JSON\"}");
            return;
        }
        
        String message = doc["message"];
        bool forceMovement = doc["force_movement"] | false;
        
        if (message.length() == 0) {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"Empty message\"}");
            return;
        }
        
        // Send to splitflap
        char buf[message.length() + 1];
        message.toCharArray(buf, sizeof(buf));
        
        splitflap_task_.showString(buf, message.length(), forceMovement);
        
        // Add to history
        addToHistory(message);
        
        char logBuf[200];
        snprintf(logBuf, sizeof(logBuf), "Web request: displaying '%s' (force: %s)", buf, forceMovement ? "true" : "false");
        logger_.log(logBuf);
        
        server_.send(200, "application/json", "{\"success\": true, \"message\": \"Text sent to display\"}");
    });
    
    server_.on("/calibrate", HTTP_POST, [this]() {
        if (!checkAuthentication()) return;
        // Trigger recalibration
        splitflap_task_.resetAll();
        
        logger_.log("Web request: triggered recalibration");
        
        server_.send(200, "application/json", "{\"success\": true, \"message\": \"Recalibration started\"}");
    });
    
    // History endpoints
    server_.on("/history", HTTP_GET, [this]() {
        if (!checkAuthentication()) return;
        String historyJson = getHistoryJSON();
        server_.send(200, "application/json", historyJson);
    });
    
    // Admin endpoints
    server_.on("/admin/status", HTTP_GET, [this]() {
        if (!checkAdminAuthentication()) return;
        String adminJson = getAdminStatusJSON();
        server_.send(200, "application/json", adminJson);
    });
    
    server_.on("/admin/control", HTTP_POST, [this]() {
        if (!checkAdminAuthentication()) return;
        if (!server_.hasArg("plain")) {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"No JSON data provided\"}");
            return;
        }
        
        String body = server_.arg("plain");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, body);
        
        if (error) {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"Invalid JSON\"}");
            return;
        }
        
        if (doc.containsKey("action")) {
            String action = doc["action"];
            if (action == "enable") {
                display_enabled_ = true;
                logger_.log("Display enabled by admin");
                server_.send(200, "application/json", "{\"success\": true, \"message\": \"Display enabled\"}");
            } else if (action == "disable") {
                display_enabled_ = false;
                logger_.log("Display disabled by admin");
                server_.send(200, "application/json", "{\"success\": true, \"message\": \"Display disabled\"}");
            } else {
                server_.send(400, "application/json", "{\"success\": false, \"message\": \"Unknown action\"}");
            }
        } else {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"Missing action field\"}");
        }
    });
    
    server_.on("/history", HTTP_POST, [this]() {
        if (!checkAuthentication()) return;
        if (!server_.hasArg("plain")) {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"No JSON data provided\"}");
            return;
        }
        
        String body = server_.arg("plain");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, body);
        
        if (error) {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"Invalid JSON\"}");
            return;
        }
        
        if (doc.containsKey("text")) {
            String text = doc["text"];
            addToHistory(text);
            server_.send(200, "application/json", "{\"success\": true, \"message\": \"History updated\"}");
        } else {
            server_.send(400, "application/json", "{\"success\": false, \"message\": \"Missing text field\"}");
        }
    });
    
    // Logout endpoint
    server_.on("/logout", [this]() {
        server_.sendHeader("WWW-Authenticate", "Basic realm=\"Login Required\"");
        server_.send(401, "text/plain", "Logged out");
    });
    
    // Handle 404
    server_.onNotFound([this]() {
        server_.send(404, "application/json", "{\"success\": false, \"message\": \"Not found\"}");
    });
}

String WebServerTask::getAlphabetString() {
    String alphabet = "[";
    for (int i = 0; i < NUM_FLAPS; i++) {
        if (i > 0) {
            alphabet += ",";
        }
        alphabet += "\"";
        char c = (char)flaps[i];
        alphabet += c;
        alphabet += "\"";
    }
    alphabet += "]";
    return alphabet;
}

String WebServerTask::getStatusJSON() {
    SplitflapState state = splitflap_task_.getState();
    
    String json = "{";
    json += "\"connected\": true,";
    json += "\"num_modules\": " + String(NUM_MODULES) + ",";
    json += "\"alphabet\": " + getAlphabetString() + ",";
    json += "\"mode\": \"" + String(state.mode == SplitflapMode::MODE_RUN ? "run" : "sensor_test") + "\"";
    json += "}";
    
    return json;
}

String WebServerTask::getHistoryJSON() {
    DynamicJsonDocument doc(2048);
    JsonArray historyArray = doc.createNestedArray("history");
    
    for (int i = 0; i < history_count_; i++) {
        historyArray.add(history_[i]);
    }
    
    String result;
    serializeJson(doc, result);
    return result;
}

String WebServerTask::getAdminStatusJSON() {
    DynamicJsonDocument doc(1024);
    doc["enabled"] = display_enabled_;
    doc["admin_mode"] = true;
    
    String result;
    serializeJson(doc, result);
    return result;
}

void WebServerTask::addToHistory(const String& text) {
    if (text.length() == 0) return;
    
    // Shift existing history down
    for (int i = MAX_HISTORY - 1; i > 0; i--) {
        history_[i] = history_[i - 1];
    }
    
    // Add new text at the beginning
    history_[0] = text;
    
    // Update count (max 10)
    if (history_count_ < MAX_HISTORY) {
        history_count_++;
    }
}