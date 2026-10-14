#include <3ds.h>
#include <stdio.h>
#include <arpa/inet.h>

static void logButton(u32 keys, u32 button, const char *name)
	{
	if (keys & button)
		printf("BUTTON %s PRESSED\n", name);
	}

int main(int argc, char **argv) {

	gfxInitDefault();

	PrintConsole topScreen;
	PrintConsole bottomScreen;

	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);

	// Main loop
	while (aptMainLoop())
	{
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

		printf("\x1b[1;0H3DS Mac Control v0.0.4 - made by odjegg");
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

		//Wait for VBlank
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}