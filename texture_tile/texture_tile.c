#include <stdio.h>
#include <stdint.h>
#include <memory.h>

#include "../image/image_mat.h"
#include "texture_tile.h"

static void Copy4by4Tile(ImageMat* mat, int x, int y, uint8_t* pTextureTile, uint32_t* ttOffset);
static void Copy8by8Tile(ImageMat* mat, int x, int y, uint8_t* pTextureTile, uint32_t* ttOffset);
static void Copy16by16Tile(ImageMat* mat, int x, int y, uint8_t* pTextureTile, uint32_t* ttOffset);
static void Decode4by4Tile(uint8_t* pTextureTile, uint32_t* ttOffset, ImageMat* mat, int x, int y);
static void Decode8by8Tile(uint8_t* pTextureTile, uint32_t* ttOffset, ImageMat* mat, int x, int y);
static void Decode16by16Tile(uint8_t* pTextureTile, uint32_t* ttOffset, ImageMat* mat, int x, int y);

int ConvertBitmapToTextureTile(ImageMat* imageMat, uint8_t *pTextureTile)
{
    int width = imageMat->width;
    int height = imageMat->height;

    if (((width & 0xF) != 0) || ((height & 0xF) != 0))
    {
        // The width/height is not divisible by 16, exit
        printf("The width(%d)/height(%d) of the bitmap is not divisible by 16, exit\n",
                width, height);
        return -1;
    }

    uint32_t ttOffset = 0;
    for (int y = 0; y < height; y += 16)
    {
        for (int x = 0; x < width; x += 16)
        {
            Copy16by16Tile(imageMat, x, y, pTextureTile, &ttOffset);
        }
    }

    return 0;
}

int ConvertTextureTileToBitmap(uint8_t *pTextureTile, uint32_t width, uint32_t height, ImageMat** pImageMat)
{
    if (((width & 0xF) != 0) || ((height & 0xF) != 0))
    {
        // The width/height is not divisible by 16, exit
        printf("The width(%d)/height(%d) of the bitmap is not divisible by 16, exit\n",
                width, height);
        return -1;
    }

    ImageMat* imageMat = NewImageMat(width, height);
    *pImageMat = imageMat;

    uint32_t ttOffset = 0;
    for (int y = 0; y < height; y += 16)
    {
        for (int x = 0; x < width; x += 16)
        {
            Decode16by16Tile(pTextureTile, &ttOffset, imageMat, x, y);
        }
    }

    return 0;
}

static void Copy4by4Tile(ImageMat* mat, int x, int y, uint8_t* pTextureTile, uint32_t* ttOffset)
{
    uint32_t pxOffset = ((y) * mat->width + x) * sizeof(uint32_t);

    // Read a 4 by 4 tile
    for (uint8_t py = 0; py < 4; py++)
    {
        memcpy(pTextureTile + *ttOffset, mat->pData + pxOffset, 4 * sizeof(uint32_t));
        *ttOffset += 4 * sizeof(uint32_t);
        pxOffset += mat->width * sizeof(uint32_t);
    }
}

static void Copy8by8Tile(ImageMat* mat, int x, int y, uint8_t* pTextureTile, uint32_t* ttOffset)
{
    for (uint8_t py = 0; py < 8; py += 4)
    {
        for (uint8_t px = 0; px < 8; px += 4)
        {
            Copy4by4Tile(mat, x + px, y + py, pTextureTile, ttOffset);
        }
    }
}

static void Copy16by16Tile(ImageMat* mat, int x, int y, uint8_t* pTextureTile, uint32_t* ttOffset)
{
    for (uint8_t py = 0; py < 16; py += 8)
    {
        for (uint8_t px = 0; px < 16; px += 8)
        {
            Copy8by8Tile(mat, x + px, y + py, pTextureTile, ttOffset);
        }
    }    
}

static void Decode4by4Tile(uint8_t* pTextureTile, uint32_t* ttOffset, ImageMat* mat, int x, int y)
{
    uint32_t pxOffset = ((y) * mat->width + x) * sizeof(uint32_t);

    // Read a 4 by 4 tile
    for (uint8_t py = 0; py < 4; py++)
    {
        memcpy(mat->pData + pxOffset, pTextureTile + *ttOffset, 4 * sizeof(uint32_t));
        *ttOffset += 4 * sizeof(uint32_t);
        pxOffset += mat->width * sizeof(uint32_t);
    }
}

static void Decode8by8Tile(uint8_t* pTextureTile, uint32_t* ttOffset, ImageMat* mat, int x, int y)
{
    for (uint8_t py = 0; py < 8; py += 4)
    {
        for (uint8_t px = 0; px < 8; px += 4)
        {
            Decode4by4Tile(pTextureTile, ttOffset, mat, x + px, y + py);
        }
    }
}

static void Decode16by16Tile(uint8_t* pTextureTile, uint32_t* ttOffset, ImageMat* mat, int x, int y)
{
    for (uint8_t py = 0; py < 16; py += 8)
    {
        for (uint8_t px = 0; px < 16; px += 8)
        {
            Decode8by8Tile(pTextureTile, ttOffset, mat, x + px, y + py);
        }
    }    
}
