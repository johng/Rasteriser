//
// Created by John Griffith on 24/03/2017.
//

#ifndef COMPUTER_GRAPHICS_TGAIMAGE_H
#define COMPUTER_GRAPHICS_TGAIMAGE_H


#include <tic.h>
#include <cstdint>

typedef struct _TgaHeader
{
		uint8_t IDLength;        /* 00h  Size of Image ID field */
		uint8_t ColorMapType;    /* 01h  Color map type */
		uint8_t ImageType;       /* 02h  Image type code */
		uint32_t CMapStart;       /* 03h  Color map origin */
		uint32_t CMapLength;      /* 05h  Color map length */
		uint8_t CMapDepth;       /* 07h  Depth of color map entries */
		uint32_t XOffset;         /* 08h  X origin of image */
		uint32_t YOffset;         /* 0Ah  Y origin of image */
		uint32_t Width;           /* 0Ch  Width of image */
		uint32_t Height;          /* 0Eh  Height of image */
		uint8_t PixelDepth;      /* 10h  Image pixel size */
		uint8_t ImageDescriptor; /* 11h  Image descriptor byte */
} TGAHEAD;

class TGAImage {

private:
		width;
		height;

public:
		void ReadTGAImage(char * filename );
		int getWidth();
		int getHeight();


};



#endif //COMPUTER_GRAPHICS_TGAIMAGE_H
