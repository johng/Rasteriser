//
// Created by John Griffith on 24/03/2017.
//

#ifndef COMPUTER_GRAPHICS_TGAIMAGE_H
#define COMPUTER_GRAPHICS_TGAIMAGE_H


#include <tic.h>
#include <cstdint>

struct _TgaHeader
{
		uint8_t IDLength;
		uint8_t ColorMapType;
		uint8_t ImageType;

    uint8_t CMapStart;
    uint16_t CMapLength;
    uint8_t CMapDepth;

    uint16_t XOffset;
    uint16_t YOffset;
    uint16_t Width;
    uint16_t Height;

    uint8_t PixelDepth;
		uint8_t ImageDescriptor;
} ;

struct TGAPixel {
    char * ptr;
    char size ;
    TGAPixel( char * ptr, char size);
};




class TGAImport {

private:
    char * texture_data;
		int width;
		int height;
    int bytesPerPixel;


public:
		bool ReadTGAImage(char * filename );
		int GetWidth();
		int GetHeight();
    void Mirror_horizontally();
    void Mirror_vertically();
    TGAPixel Get(int x, int y);

};



#endif //COMPUTER_GRAPHICS_TGAIMAGE_H
