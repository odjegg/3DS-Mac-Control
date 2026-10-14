#include <3ds.h>
#include <stdio.h>

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

		printf("\x1b[1;0H3DS Mac Control v0.0.2 - made by odjegg]");
		printf("\x1b[2;0HPress START to exit");

		printf("\x1b[3;0HTouch X coordinate: %i       ",touch.px);
		printf("\x1b[4;0HTouch Y coordinate: %i       ",touch.py);

		printf("\x1b[6;0HCircle X coordinate: %i       ",circle.dx);
		printf("\x1b[7;0HCircle Y coordinate: %i       ",circle.dy);

		if (kHeld & KEY_TOUCH)
			printf("\x1b[5;0HTouch active: TRUE ");
		else
			printf("\x1b[5;0HTouch active: FALSE");

		if (kHeld & (KEY_CPAD_UP | KEY_CPAD_DOWN | KEY_CPAD_LEFT | KEY_CPAD_RIGHT))
			printf("\x1b[8;0HCircle active: TRUE ");
		else
			printf("\x1b[8;0HCircle active: FALSE");

		if (kHeld & KEY_A)
			printf("\x1b[9;0HA key pressed: TRUE ");
		else
			printf("\x1b[9;0HA key pressed: FALSE");
		if (kHeld & KEY_B)
			printf("\x1b[10;0HB key pressed: TRUE ");
		else
			printf("\x1b[10;0HB key pressed: FALSE");
		if (kHeld & KEY_X)
			printf("\x1b[11;0HX key pressed: TRUE ");
		else
			printf("\x1b[11;0HX key pressed: FALSE");
		if (kHeld & KEY_Y)
			printf("\x1b[12;0HY key pressed: TRUE ");
		else
			printf("\x1b[12;0HY key pressed: FALSE");
		if (kHeld & KEY_L)
			printf("\x1b[13;0HL shoulder pressed: TRUE ");
		else
			printf("\x1b[13;0HL shoulder pressed: FALSE");
		if (kHeld & KEY_R)
			printf("\x1b[14;0HR shoulder pressed: TRUE ");
		else
			printf("\x1b[14;0HR shoulder pressed: FALSE");

		consoleSelect(&bottomScreen);

		if (kDown & KEY_A)
			printf("BUTTON A PRESSED\n");
		if (kDown & KEY_B)
			printf("BUTTON B PRESSED\n");

		if (kDown & KEY_START) break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}