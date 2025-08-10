import socket
import keyboard
import time
import os
#
# --------------------------r
# Configuration
# --------------------------
PI_IP = '192.168.1.187'
PI_PORT = 8000

# --------------------------
# Control State Variables
# --------------------------
pitch = 0
roll = 0
yaw = 0
throttle = 1000

max_angle = 30
step_size = 5
throttle_step = 50
max_throttle = 2000
min_throttle = 1000

# --------------------------
# Connect to Raspberry Pi
# --------------------------
try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((PI_IP, PI_PORT))
    print(f"[✓] Connected to Raspberry Pi at {PI_IP}:{PI_PORT}")
except Exception as e:
    print(f"[✗] Connection failed: {e}")
    exit(1)

# --------------------------
# Helper Functions
# --------------------------
def clamp(val, min_val, max_val):
    return max(min_val, min(val, max_val))

def send_command(axis, value):
    axis = axis.upper()
    message = f"{axis}:{value}\n"
    try:
        sock.send(message.encode())
        print(f"Sending to STM32: {message.strip()}")
    except:
        print("[✗] Lost connection to Raspberry Pi.")
        exit(1)

def print_ui():
    os.system('cls' if os.name == 'nt' else 'clear')
    print("[ Drone Control - Terminal UI ]")
    print(f"PITCH   : {pitch:>3}")
    print(f"ROLL    : {roll:>3}")
    print(f"YAW     : {yaw:>3}")
    print(f"THROTTLE: {throttle:>4}")
    print("-" * 30)
    print("W/S = Pitch ±")
    print("A/D = Roll ±")
    print("Q/E = Yaw ±")
    print("↑/↓ = Throttle ±")
    print("R = Reset P/R/Y")
    print("L = Auto-land")
    print("ESC = Exit")

# --------------------------
# Main Loop
# --------------------------
print_ui()
while True:
    time.sleep(0.05)

    if keyboard.is_pressed('w'):
        pitch = clamp(pitch + step_size, -max_angle, max_angle)
        send_command("PITCH", pitch)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('s'):
        pitch = clamp(pitch - step_size, -max_angle, max_angle)
        send_command("PITCH", pitch)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('a'):
        roll = clamp(roll - step_size, -max_angle, max_angle)
        send_command("ROLL", roll)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('d'):
        roll = clamp(roll + step_size, -max_angle, max_angle)
        send_command("ROLL", roll)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('q'):
        yaw = clamp(yaw - step_size, -max_angle, max_angle)
        send_command("YAW", yaw)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('e'):
        yaw = clamp(yaw + step_size, -max_angle, max_angle)
        send_command("YAW", yaw)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('up'):
        throttle = clamp(throttle + throttle_step, min_throttle, max_throttle)
        send_command("THROTTLE", throttle)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('down'):
        throttle = clamp(throttle - throttle_step, min_throttle, max_throttle)
        send_command("THROTTLE", throttle)
        print_ui()
        time.sleep(0.2)

    elif keyboard.is_pressed('r'):
        pitch = roll = yaw = 0
        send_command("PITCH", pitch)
        send_command("ROLL", roll)
        send_command("YAW", yaw)
        print_ui()
        time.sleep(0.5)

    elif keyboard.is_pressed('l'):
        print("[!] Auto-landing initiated...")
        while throttle > min_throttle:
            throttle -= 25
            throttle = clamp(throttle, min_throttle, max_throttle)
            send_command("THROTTLE", throttle)
            print_ui()
            time.sleep(0.3)
        print("[✓] Drone landed and throttle minimized.")
        time.sleep(1)

    elif keyboard.is_pressed('esc'):
        print("[!] Exiting...")
        break

# Cleanup
sock.close()
#w