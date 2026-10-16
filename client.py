import socket
from pynput.mouse import Button, Controller

# Network stuff
ip = "0.0.0.0"
port = 5000
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((ip, port))

print("Python client v0.0.4 for 3DS-Mac-Control v0.1.3")
print("Listening on UDP port 5000")

mouse = Controller()
run = True
last_touch = None

while run == True:
    data, addr = sock.recvfrom(1024)
    text = data.decode(errors="replace").strip()
    print(text)

    # Absolute touch
    #if text.startswith("TOUCH COORDINATES:"):
        #coords_part = text.split(":", 1)[1].strip()
        #x_str, y_str = coords_part.split(",")
        #touchpx = int(x_str.strip())
        #touchpx *= 5
        #touchpy = int(y_str.strip())
        #touchpy *= 5
        #print("Touch:", touchpx, touchpy)
        #mouse.position = (touchpx, touchpy)

    # Relative touch
    if text.startswith("TOUCH COORDINATES:"):
        coords_part = text.split(":", 1)[1].strip()
        x_str, y_str = coords_part.split(",")
        x = int(x_str.strip())
        y = int(y_str.strip())

        if last_touch is None:
            last_touch = (x, y)
        else:
            dx_raw = x - last_touch[0]
            dy_raw = y - last_touch[1]
            
            if abs(dx_raw) > 60 or abs(dy_raw) > 60: # New gesture
                last_touch = (x, y)
            else:
                sensitivity = 5.0 # Change this
                dx = int(dx_raw * sensitivity)
                dy = int(dy_raw * sensitivity)

                if dx != 0 or dy != 0:
                    mouse.move(dx, dy)

                last_touch = (x, y)

    if text.startswith("CIRCLE COORDINATES:"):
        coords_part = text.split(":", 1)[1].strip()
        x_str, y_str = coords_part.split(",")
        scale = 0.3
        circledx = int(x_str.strip())
        circledx *= scale
        circledy = int(y_str.strip())
        circledy *= scale
        mouse.move(circledx, -circledy)

    if "BUTTON A PRESSED" in text:
        mouse.click(Button.right)
    if "BUTTON Y PRESSED" in text:
        mouse.click(Button.left)
    if "BUTTON L PRESSED" in text:
        mouse.click(Button.left)
    if "BUTTON R PRESSED" in text:
        mouse.click(Button.right)

    if "EXIT" in text:
        run = False