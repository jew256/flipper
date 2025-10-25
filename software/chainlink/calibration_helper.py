#!/usr/bin/env python3

import time
from splitflap_proto import ask_for_serial_port, splitflap_context

def calibration_mode():
    """Interactive calibration mode to fix position offsets"""
    
    port = ask_for_serial_port()
    
    with splitflap_context(port) as sf:
        num_modules = sf.get_num_modules()
        print(f"Connected to splitflap with {num_modules} modules")
        print(f"Available characters: {sf.get_alphabet()}")
        
        print("\n=== CALIBRATION MODE ===")
        print("This will help you fix the position offset issue where")
        print("the estimated position doesn't match the physical position.")
        print()
        
        # Show current status
        print("Current display status:")
        sf.set_text("     ")  # Show all spaces first
        time.sleep(2)
        
        # Test each module individually
        for module_id in range(num_modules):
            print(f"\n--- Testing Module {module_id} ---")
            
            # Show the module number
            test_text = [" "] * num_modules
            test_text[module_id] = str(module_id)
            sf.set_text("".join(test_text))
            
            print(f"Module {module_id} should show '{module_id}'")
            print("Commands:")
            print("  [1] Fine adjust +1/10 step")
            print("  [2] Fine adjust +1/2 step") 
            print("  [3] Fine adjust -1/10 step")
            print("  [4] Fine adjust -1/2 step")
            print("  [s] Set current position as offset")
            print("  [n] Next module")
            print("  [q] Quit calibration")
            
            while True:
                try:
                    cmd = input("Command: ").strip().lower()
                    
                    if cmd == '1':
                        sf.increase_offset_tenth(module_id)
                        print("Adjusted +1/10 step")
                    elif cmd == '2':
                        sf.increase_offset_half(module_id)
                        print("Adjusted +1/2 step")
                    elif cmd == '3':
                        # Decrease by increasing offset in opposite direction
                        for _ in range(10):  # 10 * 1/10 = 1 full step
                            sf.increase_offset_tenth(module_id)
                        print("Adjusted -1/10 step")
                    elif cmd == '4':
                        # Decrease by increasing offset in opposite direction  
                        for _ in range(2):  # 2 * 1/2 = 1 full step
                            sf.increase_offset_half(module_id)
                        print("Adjusted -1/2 step")
                    elif cmd == 's':
                        sf.set_offset(module_id)
                        print(f"Set current position as offset for module {module_id}")
                        break
                    elif cmd == 'n':
                        break
                    elif cmd == 'q':
                        return
                    else:
                        print("Invalid command")
                        
                except KeyboardInterrupt:
                    print("\nExiting...")
                    return
        
        # Save all offsets
        print("\nSaving all offsets...")
        sf.save_all_offsets()
        print("Calibration complete!")

def quick_test():
    """Quick test to see current calibration status"""
    
    port = ask_for_serial_port()
    
    with splitflap_context(port) as sf:
        print(f"Connected to splitflap with {sf.get_num_modules()} modules")
        
        # Test with numbers
        test_strings = ["01234", "56789", "     "]
        
        for test_str in test_strings:
            print(f"\nDisplaying: '{test_str}'")
            sf.set_text(test_str)
            time.sleep(3)

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        quick_test()
    else:
        calibration_mode()






