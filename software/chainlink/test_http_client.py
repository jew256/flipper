#!/usr/bin/env python3

import requests
import json
import time
import argparse

def test_server(host='localhost', port=8080):
    """Test the splitflap HTTP server"""
    
    base_url = f"http://{host}:{port}"
    
    print(f"Testing splitflap HTTP server at {base_url}")
    
    # Test 1: Health check
    print("\n1. Testing health check...")
    try:
        response = requests.get(f"{base_url}/health")
        print(f"   Status: {response.status_code}")
        print(f"   Response: {response.json()}")
    except Exception as e:
        print(f"   Error: {e}")
        return False
    
    # Test 2: Get status
    print("\n2. Testing status endpoint...")
    try:
        response = requests.get(f"{base_url}/status")
        print(f"   Status: {response.status_code}")
        status = response.json()
        print(f"   Connected: {status.get('connected', False)}")
        if status.get('connected'):
            print(f"   Modules: {status.get('num_modules', 'Unknown')}")
            print(f"   Serial Port: {status.get('serial_port', 'Unknown')}")
    except Exception as e:
        print(f"   Error: {e}")
        return False
    
    # Test 3: Send text messages
    test_messages = ["hello", "12345", "test ", "     "]
    
    for i, message in enumerate(test_messages, 3):
        print(f"\n{i}. Testing text message: '{message}'")
        try:
            data = {"message": message}
            response = requests.post(f"{base_url}/text", json=data)
            result = response.json()
            print(f"   Status: {response.status_code}")
            print(f"   Success: {result.get('success', False)}")
            print(f"   Message: {result.get('message', 'No message')}")
            time.sleep(2)  # Wait between messages
        except Exception as e:
            print(f"   Error: {e}")
    
    # Test 4: Test with force movement
    print(f"\n{len(test_messages) + 3}. Testing force movement...")
    try:
        data = {"message": "hello", "force_movement": True}
        response = requests.post(f"{base_url}/text", json=data)
        result = response.json()
        print(f"   Status: {response.status_code}")
        print(f"   Success: {result.get('success', False)}")
        print(f"   Message: {result.get('message', 'No message')}")
    except Exception as e:
        print(f"   Error: {e}")
    
    print("\nTest completed!")
    return True

def send_message(host='localhost', port=8080, message='hello'):
    """Send a single message to the server"""
    base_url = f"http://{host}:{port}"
    
    try:
        data = {"message": message}
        response = requests.post(f"{base_url}/text", json=data)
        result = response.json()
        
        print(f"Status: {response.status_code}")
        print(f"Success: {result.get('success', False)}")
        print(f"Message: {result.get('message', 'No message')}")
        
        return result.get('success', False)
        
    except Exception as e:
        print(f"Error: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description='Test Splitflap HTTP Server')
    parser.add_argument('--host', default='localhost', help='Server host (default: localhost)')
    parser.add_argument('--port', type=int, default=8080, help='Server port (default: 8080)')
    parser.add_argument('--message', help='Send a single message instead of running full test')
    
    args = parser.parse_args()
    
    if args.message:
        send_message(args.host, args.port, args.message)
    else:
        test_server(args.host, args.port)

if __name__ == '__main__':
    main()






