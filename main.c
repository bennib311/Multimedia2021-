#include <assert.h>
#include <stdlib.h>

#include "bitmap.h"

static void manipulate(bitmap_pixel_hsv_t* pixels, uint32_t count)
{
		for (uint32_t x = 0; x < count; x++)
		{

			bitmap_pixel_hsv_t* pix = &pixels[x];

			if ((pix->v + 50) > 255) {
				pix->v = 255;
			} 
			else if ((pix->v + 50) < 0) {
				pix->v = 0;
			}
			else {
				pix->v = (pix->v + 50);
			}
		}	
}

int main(void)
{
	//Read bitmap pixels, convert to HSV:
	bitmap_pixel_hsv_t* pixels;
	uint32_t widthPx, heightPx;

	bitmap_error_t error = bitmapReadPixels("sails.bmp", (bitmap_pixel_t**)&pixels, &widthPx, &heightPx, BITMAP_COLOR_SPACE_HSV);
	assert(error == BITMAP_ERROR_SUCCESS);

	manipulate(pixels, widthPx * heightPx);

	//Write bitmap pixels, assume HSV in source:
	bitmap_parameters_t params;

	params.bottomUp = BITMAP_BOOL_TRUE;
	params.widthPx = widthPx;
	params.heightPx = heightPx;
	params.colorDepth = BITMAP_COLOR_DEPTH_24;
	params.compression = BITMAP_COMPRESSION_NONE;
	params.dibHeaderFormat = BITMAP_DIB_HEADER_INFO;
	params.colorSpace = BITMAP_COLOR_SPACE_HSV;

	error = bitmapWritePixels("test.mod.bmp", BITMAP_BOOL_TRUE, &params, (bitmap_pixel_t*)pixels);
	assert(error == BITMAP_ERROR_SUCCESS);

	//Free the pixel array:
	free(pixels);

	return 0;
}