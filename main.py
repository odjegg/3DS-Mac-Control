import socket
from pynput.mouse import Button, Controller

# Network stuff
ip = "0.0.0.0"
port = 5000
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((ip, port))

print("Python client for 3DS-Mac-Control v0.1")
print("Listening on UDP port 5000")

mouse = Controller()
run = True
while run == True:
    data, addr = sock.recvfrom(1024)
    text = data.decode(errors="replace").strip()
    print(text)

    if text.startswith("TOUCH COORDINATES:"):
        coords_part = text.split(":", 1)[1].strip()
        x_str, y_str = coords_part.split(",")
        touchpx = int(x_str.strip())
        touchpx *= 5
        touchpy = int(y_str.strip())
        touchpy *= 5
        print("Touch:", touchpx, touchpy)
        mouse.position = (touchpx, touchpy)

    if text.startswith("CIRCLE COORDINATES:"):
        coords_part = text.split(":", 1)[1].strip()
        x_str, y_str = coords_part.split(",")
        circledx = int(x_str.strip())
        circledx *= 0.5
        circledy = int(y_str.strip())
        circledy *= 0.5
        print("Circle:", circledx, circledy)
        mouse.move(circledx, -circledy)

    if "BUTTON A PRESSED" in text:
        mouse.click(Button.right)
    if "BUTTON Y PRESSED" in text:
        mouse.click(Button.left)

    if "EXIT" in text:
        run = False


