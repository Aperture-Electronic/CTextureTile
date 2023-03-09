#ifndef _TEXTURE_TILE_H_
#define _TEXTURE_TILE_H_

#include <stdint.h>

#include "../image/image_mat.h"

int ConvertBitmapToTextureTile(ImageMat* imageMat, uint8_t *pTextureTile);
int ConvertTextureTileToBitmap(uint8_t *pTextureTile, uint32_t width, uint32_t height, ImageMat** imageMat);

#endif
