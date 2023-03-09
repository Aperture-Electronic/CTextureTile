#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#include "image/image_mat.h"
#include "bmp/bmp.h"
#include "bmp/bmp_reader.h"
#include "bmp/bmp_writer.h"
#include "texture_tile/texture_tile.h"

void PrintHelp(char* programName);

int main(int argc, char* argv[])
{
    printf("Texture Tile Converter\n");

    char* programName;
    programName = argv[0];

    if (argc < 3)
    {
        // Print help when no enough arguments input
        PrintHelp(programName);
        return 0;
    }

    char *inputFileName = NULL, *outputFileName = NULL;
    uint8_t inputFileSetted = 0;
    uint8_t convertBack = 0;
    
    for (int i = 1; i < argc; i++)
    {
        char* argument = argv[i];
        if (strcmp(argument, "-r") == 0)
        {
            convertBack = 1;
        }
        else if (strcmp(argument, "-h") == 0)
        {
            PrintHelp(programName);
        }
        else if (!inputFileSetted)
        {
            inputFileName = argument;
            inputFileSetted = 1;
        }
        else
        {
            outputFileName = argument;
        }
    }

    if ((inputFileName == NULL) || (outputFileName == NULL))
    {
        // Arguments error
        PrintHelp(programName);
        return 0;
    }

    if (convertBack)
    {
        printf("Converting back...\n");

        FILE *fpInputFile;
        fpInputFile = fopen(inputFileName, "rb");

        if (fpInputFile == NULL)
        {
            printf("Can not open the input texture tile file %s\n", inputFileName);
        }

        uint32_t width, height;
        fread(&width, sizeof(uint32_t), 1, fpInputFile);
        fread(&height, sizeof(uint32_t), 1, fpInputFile);

        uint32_t length = width * height * sizeof(uint32_t);
        uint8_t* tileMapBuffer = (uint8_t*)malloc(length);
        Bitmap* outputBitmap = NULL;
        NewBitmap32bpp(&outputBitmap, width, height, "");

        size_t rdPointer = 0;
        do
        {
            rdPointer += fread(tileMapBuffer + rdPointer, sizeof(uint8_t), length - rdPointer, fpInputFile);
        } while (rdPointer != length);
        
        fclose(fpInputFile);

        ConvertTextureTileToBitmap(tileMapBuffer, width, height, &outputBitmap->image);
        free(tileMapBuffer);

        BitmapWriteError errWrite = WriteBitmap(outputBitmap, outputFileName);
        if (errWrite != BMPW_ERR_OK)
        {
            printf("Can not write the Bitmap file %s (%d)\n", outputFileName, errWrite);
            return errWrite;
        }

        printf("Done.\n");
    }
    else
    {
        printf("Converting...\n");

        Bitmap* inputBitmap = NULL;
        BitmapReadError errRead = ReadBitmap(inputFileName, &inputBitmap);
        if (errRead != BMPR_ERR_OK)
        {
            printf("Can not read the Bitmap file %s (%d)\n", inputFileName, errRead);
            return errRead;
        }

        ImageMat* mat = inputBitmap->image;
        uint32_t length = mat->width * mat->height * sizeof(uint32_t);
        uint8_t* tileMapBuffer = (uint8_t*)malloc(length);
        ConvertBitmapToTextureTile(mat, tileMapBuffer);

        FILE* fpOutputFile;
        fpOutputFile = fopen(outputFileName, "wb+");

        fwrite(&mat->width, sizeof(uint32_t), 1, fpOutputFile);
        fwrite(&mat->height, sizeof(uint32_t), 1, fpOutputFile);

        uint32_t wrPointer = 0;
        do
        {
            wrPointer += fwrite(tileMapBuffer + wrPointer, sizeof(uint8_t), length - wrPointer, fpOutputFile);
        } while (wrPointer != length);

        free(tileMapBuffer);
        fclose(fpOutputFile);

        printf("Done.\n");
    }

    return 0;
}

void PrintHelp(char* programName)
{
    printf("This program can convert the bitmap image to store it as continuous 16x16 texture tiles.\n");
    printf("Or, convert the texture tiles file back to Bitmap.\n");
    printf("Usage: %s [-r][-h] <input_file> <output_file>\n", programName);
    printf("\t-r:  Convert back from a texture tiles file\n");
    printf("\t-h:  Show this help\n");
    printf("\t<input_file>:  Input file.\n");
    printf("\t               For convert to texture tiles file, this must be a Bitmap.\n");
    printf("\t               For convert back, this must be a Texture Tiles File.\n");
    printf("\t<output_file>: Output file.\n");
    printf("\n");
    printf("NOTE: The format of output texture tiles file: \n");
    printf("\t    There are 2 of 32-bit binary fields at the head of file, \n");
    printf("\t    indicated the width and height of the image (in pixel).\n");
    printf("\t    The 32-bit binary fields are stored as little-endian.\n");
    printf("\t    And the RGBA pixels are follow the head.\n");
    printf("\t    The fields of each pixel are arranged in order of R, G, B, A.\n");
}
