#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "bitmap.h"

static void manipulate(bitmap_pixel_rgb_t* pixels1, bitmap_pixel_rgb_t* pixels2, uint32_t count, float value)
{
		for (uint32_t x = 0; x < count; x++)
		{

			bitmap_pixel_rgb_t* pix1 = &pixels1[x];
			bitmap_pixel_rgb_t* pix2 = &pixels2[x];

			pix1->r = ((value * pix1->r) + ((1-value) * pix2->r));
			pix1->g = ((value * pix1->g) + ((1-value) * pix2->g));
			pix1->b = ((value * pix1->b) + ((1-value) * pix2->b));
		}	
}

int main(int argc, char* argv[])
{
	if (argc != 4) {
		printf("Bitte geben Sie 3 Kommandozeilenparamter in der der folgenden Form an: <Dateipfad1> <Dateipfad2> <Alphablending-Faktor>\n");
		return 0;
	}

	float alph = atof(argv[3]);
	

	bitmap_pixel_rgb_t* pixels1;
	bitmap_pixel_rgb_t* pixels2;
	uint32_t widthPx1, heightPx1, widthPx2, heightPx2;

	bitmap_error_t error1 = bitmapReadPixels(argv[1], (bitmap_pixel_t**)&pixels1, &widthPx1, &heightPx1, BITMAP_COLOR_SPACE_RGB);
	assert(error1 == BITMAP_ERROR_SUCCESS);
	bitmap_error_t error2 = bitmapReadPixels(argv[2], (bitmap_pixel_t**)&pixels2, &widthPx2, &heightPx2, BITMAP_COLOR_SPACE_RGB);
	assert(error2 == BITMAP_ERROR_SUCCESS);

	if (widthPx1 != widthPx2 || heightPx1 != heightPx2) {

		printf("Die Größe der Bilder stimmt nicht überein!\n");
		free(pixels1);
		free(pixels2);
		return 0;
	}

	manipulate(pixels1, pixels2, widthPx1 * heightPx1, alph);

	bitmap_parameters_t params1, params2;

	params1.bottomUp = BITMAP_BOOL_TRUE;
	params1.widthPx = widthPx1;
	params1.heightPx = heightPx1;
	params1.colorDepth = BITMAP_COLOR_DEPTH_24;
	params1.compression = BITMAP_COMPRESSION_NONE;
	params1.dibHeaderFormat = BITMAP_DIB_HEADER_INFO;
	params1.colorSpace = BITMAP_COLOR_SPACE_RGB;

	params2.bottomUp = BITMAP_BOOL_TRUE;
	params2.widthPx = widthPx2;
	params2.heightPx = heightPx2;
	params2.colorDepth = BITMAP_COLOR_DEPTH_24;
	params2.compression = BITMAP_COMPRESSION_NONE;
	params2.dibHeaderFormat = BITMAP_DIB_HEADER_INFO;
	params2.colorSpace = BITMAP_COLOR_SPACE_RGB;

	error1 = bitmapWritePixels("ergebnis.bmp", BITMAP_BOOL_TRUE, &params1, (bitmap_pixel_t*)pixels1);
	assert(error1 == BITMAP_ERROR_SUCCESS);

	//Free the pixel array:
	free(pixels1);
	free(pixels2);

	return 0;
}