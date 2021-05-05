#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bitmap.h"

// Funktion zum Abdunkeln/Aufhellen
static void manipulate(bitmap_pixel_hsv_t* pixels, uint32_t count, float d)
{
		for (uint32_t x = 0; x < count; x++)
		{
			bitmap_pixel_hsv_t* pix = &pixels[x];

			if (d >= 0) {
				pix->v += (255 - pix->v) * d;
			}
			else {
				pix->v += pix->v * d;
			}

			
		}
}


int main(int argc, char* argv[])
{

	//Kontrolle der richtigen Benutzereingabe
	if (argc != 2) {
		printf("Bitte geben Sie (ausschließlich) den Parameter d als Kommandozeilenparameter an.\n");
		return 0;
	}

	float d = atof(argv[1]);

	if (d < -1 || d > 1) {

		printf("Bitte geben Sie einen Wert zwischen -1 und +1 ein.\n");
		return 0;
	}

	//Read bitmap pixels, convert to HSV:
	bitmap_pixel_hsv_t* pixels;
	uint32_t widthPx, heightPx;

	bitmap_error_t error = bitmapReadPixels("test.bmp", (bitmap_pixel_t**)&pixels, &widthPx, &heightPx, BITMAP_COLOR_SPACE_HSV);
	assert(error == BITMAP_ERROR_SUCCESS);

	manipulate(pixels, widthPx * heightPx, d);

	//Write bitmap pixels, assume HSV in source:
	bitmap_parameters_t params;

	params.bottomUp = BITMAP_BOOL_TRUE;
	params.widthPx = widthPx;
	params.heightPx = heightPx;
	params.colorDepth = BITMAP_COLOR_DEPTH_24;
	params.compression = BITMAP_COMPRESSION_NONE;
	params.dibHeaderFormat = BITMAP_DIB_HEADER_INFO;
	params.colorSpace = BITMAP_COLOR_SPACE_HSV;


	error = bitmapWritePixels("test_mod.bmp", BITMAP_BOOL_TRUE, &params, (bitmap_pixel_t*)pixels);
	assert(error == BITMAP_ERROR_SUCCESS);

	//Free the pixel array:
	free(pixels);

	return 0;
}