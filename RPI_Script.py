import serial
import socket
import time

def format_command(cmd):
    """Converts 'PITCH:10' → 'P010', 'ROLL:-5' → 'R-55' """
    cmd = cmd.strip().upper()

    def encode(value):
        # Encode -5 as -55, +10 as 010
        return "{:0>3}".format(value) if value >= 0 else "-" + "{:0>2}".format(abs(value))

    if cmd.startswith("PITCH:"):
        try:
            value = int(cmd.split(":")[1])
            return "P" + encode(value)
        except:
            return None

    elif cmd.startswith("ROLL:"):
        try:
            value = int(cmd.split(":")[1])
            return "R" + encode(value)
        except:
            return None

    elif cmd.startswith("YAW:"):
        try:
            value = int(cmd.split(":")[1])
            return "Y" + encode(value)
        except:
            return None

    elif cmd.startswith("THROTTLE:"):
        try:
            value = int(cmd.split(":")[1])
            return "T" + "{:0>3}".format(value)  
        except:
            return None

    else:
        return None

# TCP setup
HOST = '0.0.0.0'
PORT = 8000
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen(1)

print(f"Waiting for connection from host PC on port {PORT}...")
conn, addr = server_socket.accept()
print(f"Connected by {addr}")

# Serial setup
ser = serial.Serial(
    port='/dev/ttyAMA0',
    baudrate=115200,
    timeout=1
)

try:
    while True:
        data = conn.recv(1024).decode().strip()
        if not data:
            continue

        command = format_command(data)
        if command and len(command) == 4:
            print(f"Sending to STM32: {command}")
            ser.write((command + "\n").encode())
            time.sleep(0.05)
        else:
            print(f"Ignored invalid input: {data}")

except Exception as e:
    print(f"Error: {e}")

finally:
    ser.close()
    conn.close()
    server_socket.close()
    print("Ports closed")
