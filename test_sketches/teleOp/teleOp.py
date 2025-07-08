import serial
import keyboard
import time

# Configure the serial port
serial_port = 'COM6' 
baud_rate = 9600  # Update baud rate as per your configuration

# Open serial connection
ser = serial.Serial(serial_port, baud_rate, timeout=0.1)
time.sleep(2)  # Wait for the serial connection to initialize

# Command mappings
commands = {
    'w': "1,1,0,0,1,0,0,0,0,0\n",  # Forward
    's': "1,1,0,0,0,1,0,0,0,0\n",  # Backward
    'a': "1,1,0,0,0,0,1,0,0,0\n",  # Rotate left
    'd': "1,1,0,0,0,0,0,1,0,0\n",  # Rotate right
}

print("Press W/A/S/D to control the robot. Press 'q' to quit.")

try:
    while True:
        for key in commands:
            if keyboard.is_pressed(key):
                ser.write(commands[key].encode())
                print(f"Sent: {commands[key].strip()}")
                time.sleep(0.1)  # Slight delay to control command rate

        if keyboard.is_pressed('q'):
            print("Exiting...")
            break

except KeyboardInterrupt:
    print("Interrupted by user")

finally:
    ser.close()
