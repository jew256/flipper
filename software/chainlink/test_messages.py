#!/usr/bin/env python3

import time
from splitflap_proto import ask_for_serial_port, splitflap_context

def test_messages():
    """Test sending various messages to the splitflap display"""
    
    # Get the serial port
    port = ask_for_serial_port()
    
    with splitflap_context(port) as sf:
        print(f"Connected to splitflap with {sf.get_num_modules()} modules")
        print(f"Available characters: {sf.get_alphabet()}")
        
        # Test messages
        test_strings = [
            "hello",
            "12345", 
            "test ",
            "calib",
            "     ",  # all spaces
            "ABCDE"
        ]
        
        for i, message in enumerate(test_strings):
            print(f"\nTest {i+1}: Sending '{message}'")
            sf.set_text(message)
            time.sleep(3)  # Wait 3 seconds between messages
        
        print("\nAll tests completed!")

def interactive_mode():
    """Interactive mode to send custom messages"""
    
    port = ask_for_serial_port()
    
    with splitflap_context(port) as sf:
        print(f"Connected to splitflap with {sf.get_num_modules()} modules")
        print(f"Available characters: {sf.get_alphabet()}")
        print("\nEnter messages to display (or 'quit' to exit):")
        
        while True:
            try:
                message = input("Message: ").strip()
                if message.lower() in ['quit', 'exit', 'q']:
                    break
                
                if message:
                    print(f"Sending: '{message}'")
                    sf.set_text(message)
                else:
                    print("Empty message, skipping...")
                    
            except KeyboardInterrupt:
                print("\nExiting...")
                break

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "interactive":
        interactive_mode()
    else:
        test_messages()






