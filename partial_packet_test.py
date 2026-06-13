import socket
import time

HOST = "127.0.0.1"
PORT = 9001  # ChannelServer 포트

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(2)
s.connect((HOST, PORT))

print("connected")

s.sendall(b"\x01")
print("sent partial header")

time.sleep(3)

try:
    data = s.recv(1)
    if data == b"":
        print("disconnected: server closed connection")
    else:
        print("received:", data)
except socket.timeout:
    print("still connected: no close detected")
except ConnectionResetError:
    print("disconnected: connection reset")

try:
    s.sendall(b"\x02\x03\x04")
    print("send after check succeeded")
except BrokenPipeError:
    print("disconnected: broken pipe")
except ConnectionResetError:
    print("disconnected: connection reset on send")

s.close()
