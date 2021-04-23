#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bitmap.h"

// Funktion zum Abdunkeln/Aufhellen
static void manipulate(bitmap_pixel_hsv_t* pixels, uint32_t count, int value)
{
		for (uint32_t x = 0; x < count; x++)
		{

			bitmap_pixel_hsv_t* pix = &pixels[x];

			if ((pix->v + value) > 255) {
				pix->v = 255;
			} 
			else if ((pix->v + value) < 0) {
				pix->v = 0;
			}
			else {
				pix->v = (pix->v + value);
			}
		}	
}

int main(int argc, char* argv[])
{

	if (argc != 4) {
		printf("Bitte geben Sie 3 Kommandozeilenparamter in der der folgenden Form an: <Dateipfad> <Funktionsparameter> <Hellichkeitswert>\n");
		return 0;
	}
	
	int manvalue;
	sscanf(argv[3], "%d", &manvalue);

	if (manvalue < -100 || manvalue > 100) {

		printf("Bitte geben Sie einen Wert zwischen -100 und +100 ein.\n");
		return 0;
	}

	//Read bitmap pixels, convert to HSV:
	bitmap_pixel_hsv_t* pixels;
	uint32_t widthPx, heightPx;

	bitmap_error_t error = bitmapReadPixels(argv[1], (bitmap_pixel_t**)&pixels, &widthPx, &heightPx, BITMAP_COLOR_SPACE_HSV);
	assert(error == BITMAP_ERROR_SUCCESS);

	if (strcmp(argv[2], "b") == 0) {

		manipulate(pixels, widthPx * heightPx, manvalue);
	}
	else {

		printf("Sorry, diese Funktion steht nicht zur Verfügung.\n");
		return 0;
	}

	//Write bitmap pixels, assume HSV in source:
	bitmap_parameters_t params;

	params.bottomUp = BITMAP_BOOL_TRUE;
	params.widthPx = widthPx;
	params.heightPx = heightPx;
	params.colorDepth = BITMAP_COLOR_DEPTH_24;
	params.compression = BITMAP_COMPRESSION_NONE;
	params.dibHeaderFormat = BITMAP_DIB_HEADER_INFO;
	params.colorSpace = BITMAP_COLOR_SPACE_HSV;


	// Zur korrekten Umbenennung der Ausgabedatei:
	char manip[50] = "\0";
	
	if (manvalue >= 0) {
		strcat(manip, "brighter_");
	}
	else {
		strcat(manip, "darker_");
	}

	int absv = abs(manvalue);
	char abss[11];
	sprintf(abss, "%d", absv);

	strcat(manip, abss);
	strcat(manip, "_");
	strcat(manip, argv[1]);

	error = bitmapWritePixels(manip, BITMAP_BOOL_TRUE, &params, (bitmap_pixel_t*)pixels);
	assert(error == BITMAP_ERROR_SUCCESS);

	//Free the pixel array:
	free(pixels);

	return 0;
}