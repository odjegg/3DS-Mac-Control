#include <3ds.h> // The main devkitpro 3ds tools
#include <stdio.h>
// Network related headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Not sure if I definitely need these
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

// The IP and Port of the computer hosting the server
#define SERVER_IP "192.168.1.204"
#define SERVER_PORT 5000
// Creating buffer to reserve memory for networking
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000 // 1mb buffer (could reduce later to save memory if needed ?)
static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

__attribute__((format(printf,1,2)))
void failExit(const char *fmt, ...);

void socShutdown() {
	printf("waiting for socExit...\n");
	socExit();
}

static void logButton(int sock, const struct sockaddr_in *serverAddress, u32 keys, u32 button, const char *name) {
	if (keys & button) {
		char message[64];
		int length = snprintf(message, sizeof(message),"BUTTON %s PRESSED\n", name);
		printf("BUTTON %s PRESSED\n", name);
	}
}

int main(int argc, char **argv) {
	int ret;

	gfxInitDefault();

	// register gfxExit to be run when app quits
	// this can help simplify error handling
	atexit(gfxExit);

	PrintConsole topScreen;
	PrintConsole bottomScreen;

	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);

	consoleSelect(&bottomScreen);

	printf ("\nMade using libctru 3dslink demo\n\n");

	// allocate buffer for SOC service
	SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if(SOC_buffer == NULL) {
		failExit("memalign: failed to allocate\n");
	}

	// Now intialise soc:u service
	if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
    	failExit("socInit: 0x%08X\n", (unsigned int)ret);
	}

	// register socShutdown to run at exit
	// atexit functions execute in reverse order so this runs before gfxExit
	atexit(socShutdown);

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

		printf("\x1b[1;0H3DS Mac Control v0.0.5 - made by odjegg");
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

		logButton(kDown, KEY_A, "A");
		logButton(kDown, KEY_B, "B");
		logButton(kDown, KEY_X, "X");
		logButton(kDown, KEY_Y, "Y");

		if (kDown & KEY_START) break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		// Wait for VBlank
		gspWaitForVBlank();
	}

	// Exit socket and free buffer space
	close(sock);

	gfxExit();
	return 0;
}

void failExit(const char *fmt, ...) {

	if(sock>0) close(sock);
	if(csock>0) close(csock);

	va_list ap;

	printf(CONSOLE_RED);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf(CONSOLE_RESET);
	printf("\nPress START to exit\n");

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) exit(0);
	}
}