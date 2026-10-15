#include <3ds.h> // The main devkitpro 3ds tools
#include <stdio.h>
#include <string.h>

// C socket API
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h> // Socket close()
#include <malloc.h> // linearAlloc


// The IP and Port of the computer hosting the server
#define SERVER_IP "192.168.1.204" // REPLACE WITH YOUR PC'S LOCAL IP!!
#define SERVER_PORT 5000
// Creating buffer to reserve memory for networking
#define SOC_ALIGN 0x1000 // 4096 byte alignment
#define SOC_BUFFERSIZE  0x100000 // 1mb buffer (could reduce later to save memory if needed ?)

static void logButton(int sock, const struct sockaddr_in *serverAddress, u32 keys, u32 button, const char *name) {
	if (keys & button) {
		printf("BUTTON %s PRESSED\n", name); // Log for bottom screen
		// Send a message if socket is valid and there is a destination
		if (sock >= 0 && serverAddress != NULL) {
            char message[64];
            int length = snprintf(message, sizeof(message), "BUTTON %s PRESSED\n", name);

            if (length > 0) {
                sendto(sock, message, (size_t)length,0, (const struct sockaddr *)serverAddress, sizeof(*serverAddress));
            } else {
				printf("No connection");
			}
        }
	}
}

static void logTouch(int sock, const struct sockaddr_in *serverAddress, touchPosition touch) {
	printf("TOUCH COORDINATES: %i, %i\n", touch.px, touch.py); // Log for bottom screen
	// Send a message if socket is valid and there is a destination
	if (sock >= 0 && serverAddress != NULL) {
        char message[64];
        int length = snprintf(message, sizeof(message), "TOUCH COORDINATES: %i, %i\n", touch.px, touch.py);

        if (length > 0) {
            sendto(sock, message, (size_t)length,0, (const struct sockaddr *)serverAddress, sizeof(*serverAddress));
        } else {
			printf("No connection");
		}
    }
}

static void logCircle(int sock, const struct sockaddr_in *serverAddress, circlePosition circle) {
	printf("CIRCLE COORDINATES: %i, %i\n", circle.dx, circle.dy); // Log for bottom screen
	// Send a message if socket is valid and there is a destination
	if (sock >= 0 && serverAddress != NULL) {
        char message[64];
        int length = snprintf(message, sizeof(message), "CIRCLE COORDINATES: %i, %i\n", circle.dx, circle.dy);

        if (length > 0) {
            sendto(sock, message, (size_t)length,0, (const struct sockaddr *)serverAddress, sizeof(*serverAddress));
        } else {
			printf("No connection");
		}
    }
}

int main(int argc, char **argv) {
	gfxInitDefault();

	PrintConsole topScreen;
	PrintConsole bottomScreen;

	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);

	// Allocate buffer for SOC service
	u32 *socBuffer = (u32 *)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if (socBuffer == NULL) {
        consoleSelect(&bottomScreen);
        printf("Failed to allocate SOC buffer\n");
        gfxExit();
        return 1;
    }

	Result result = socInit(socBuffer, SOC_BUFFERSIZE);

	if (R_FAILED(result))
    {
        consoleSelect(&bottomScreen);
        printf("socInit failed: 0x%08lX\n", result);
        linearFree(socBuffer);
        gfxExit();
        return 1;
    }

	consoleSelect(&bottomScreen);
    printf("Starting connection...\n");

    // Prepare the server address
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port   = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        consoleSelect(&bottomScreen);
        printf("Failed to create UDP socket\n");
    }

	// Main loop
	while (aptMainLoop()) {
		hidScanInput();
		
		touchPosition touch;
		hidTouchRead(&touch);

		circlePosition circle;
		hidCircleRead(&circle);

		u32 kHeld = hidKeysHeld();
		u32 kDown = hidKeysDown();

		consoleSelect(&topScreen);

		struct in_addr ip;
		struct in_addr netmask;
		struct in_addr broadcast;

		SOCU_GetIPInfo(&ip, &netmask, &broadcast);

		printf("\x1b[1;0H3DS Mac Control v0.1.3 - made by odjegg");
		printf("\x1b[2;0HIP: %s", inet_ntoa(ip));
		printf("\x1b[3;0HPress START to exit");

		printf("\x1b[4;0HTouch X coordinate: %-4i",touch.px);
		printf("\x1b[5;0HTouch Y coordinate: %-4i",touch.py);

		printf("\x1b[8;0HCircle X coordinate: %-4i",circle.dx);
		printf("\x1b[9;0HCircle Y coordinate: %-4i",circle.dy);

		if (kHeld & KEY_TOUCH)
			printf("\x1b[6;0HTouch active: TRUE ");
		else
			printf("\x1b[6;0HTouch active: FALSE");

		printf("\x1b[11;0HA: %-5s B: %-5s X: %-5s Y: %-5s", 
				(kHeld & KEY_A) ? "DOWN": "UP",
				(kHeld & KEY_B) ? "DOWN": "UP",
				(kHeld & KEY_X) ? "DOWN": "UP",
				(kHeld & KEY_Y) ? "DOWN": "UP");

		consoleSelect(&bottomScreen);

		logButton(sock, &serverAddress, kDown, KEY_A, "A");
		logButton(sock, &serverAddress, kDown, KEY_B, "B");
		logButton(sock, &serverAddress, kDown, KEY_X, "X");
		logButton(sock, &serverAddress, kDown, KEY_Y, "Y");
		logButton(sock, &serverAddress, kDown, KEY_L, "L");
		logButton(sock, &serverAddress, kDown, KEY_R, "R");
		logButton(sock, &serverAddress, kDown, KEY_SELECT, "EXIT");
		logButton(sock, &serverAddress, kDown, KEY_START, "EXIT");

		if (kHeld & KEY_TOUCH) {
			logTouch(sock, &serverAddress, touch);
		}

		if (kHeld & (KEY_CPAD_UP | KEY_CPAD_DOWN | KEY_CPAD_LEFT | KEY_CPAD_RIGHT)) {
			logCircle(sock, &serverAddress, circle);
		}

		if (kDown & KEY_START) break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		// Wait for VBlank
		gspWaitForVBlank();
	}

	// Exit socket and free buffer space
	if (sock >= 0)
        close(sock);

    socExit();
    free(socBuffer);

	gfxExit();
	return 0;
}