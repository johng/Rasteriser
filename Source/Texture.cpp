//
// Created by John Griffith on 24/03/2017.
//

#include "Texture.h"
#include<iostream>
#include<fstream>
#include <string.h>

using namespace std;



bool Texture::ReadTGAImage(const char *filename) {


  ifstream stream;
  stream.open(filename, ios::binary);

  struct TGAHeader tgaHeader;
  stream.read((char*)&tgaHeader, sizeof(tgaHeader));


  width = tgaHeader.Width;
  height = tgaHeader.Height;
  bytesPerPixel = (int)tgaHeader.PixelDepth>>3;
  int imageType = (int)tgaHeader.ImageType;
  //todo check for valid ptr per pixel


  //ImageType
  //1,2,3 no compression, we can just read in the ptr

  int bytes = bytesPerPixel * width * height;

  texture_data = (unsigned char*)malloc(bytes);

  if(imageType == 1||imageType==2||imageType==3) {
    stream.read((char*)texture_data, bytes);
  }else if(imageType == 10 || imageType == 11 || imageType == 9){
    //RLE encoded data
    cout << "RLE Encoding\n";
    int pixelCount = width * height;
    int bytePtr = 0;
    int pixel = 0;
    while( pixel < pixelCount ){

      int count = stream.get();
      if(!stream.good()){
        cout << "Error reading RLE Encoding";
        return false;
      }

      // count = [ f | <length> ]
      // If flag is set we use RLE, e.g if count > 127
      if(count > 127) {

        count -= 127;
        unsigned char pixelInfo[4];
        for (int b = 0; b < bytesPerPixel; b++) {
          pixelInfo[b] = (unsigned char) stream.get();
        }

        for (int item = 0; item < count; item++) {
          memcpy(&texture_data[bytePtr], pixelInfo, bytesPerPixel);
          bytePtr+=bytesPerPixel;
          pixel++;
          if (pixel>pixelCount) {
            std::cerr << "Too many pixels read\n";
            return false;
          }
        }

      }else{
        count ++; //Encoding 1 less than actual. E.g start counting at 0
        for(int p = 0 ; p < count ; p ++) {
          unsigned char pixelInfo[4];

          for (int b = 0; b < bytesPerPixel; b++) {
            pixelInfo[b] = (unsigned char) stream.get();
          }

          memcpy(&texture_data[bytePtr], pixelInfo, bytesPerPixel);
          bytePtr+=bytesPerPixel;
          pixel++;

          if (pixel>pixelCount) {
            std::cerr << "Too many pixels read\n";
            return false;
          }
        }
      }
    }

    cout << "Final Pixel Ptr " << bytePtr << " tgt ptr " << bytes << endl;

  }



  if (!(tgaHeader.ImageDescriptor & 0x20)) {
    //Mirror_vertically();
  }
  if (tgaHeader.ImageDescriptor & 0x10) {
    Mirror_horizontally();
  }

  stream.close();
  return true;

}

int Texture::GetHeight(){
  return height;
};

int Texture::GetWidth(){
  return width;
};


unsigned char * Texture::Get(int x, int y) {

  if(x < 0 || x >= width || y < 0 || y >=height){
    unsigned char c = 0;
		cout << "ERROR!"  << x << "," << y <<"\n";
    return &c; //Bit of a hack, so we can return nothing when we don't have the textures
  }
  return &texture_data[bytesPerPixel * (x + y * width)];
}


void Texture::Mirror_horizontally() {

  int swaps = width>>1; //Divide by 2, ensuring we round down
  unsigned char* temp = (unsigned char*)malloc(bytesPerPixel);

  for(int h =0; h< height; h++) {
    for (int s = 0; s < swaps; s++) {

      unsigned char * p1 = Get(s, h);
      unsigned char * p2 = Get(width - (s+1), h);

      memcpy(temp,p1,bytesPerPixel);
      memcpy(p1,p2,bytesPerPixel);
      memcpy(p2,p1,bytesPerPixel);

    }
  }

  free(temp);

}


void Texture::Mirror_vertically() {

  int swaps = height>>1; //Divide by 2, ensuring we round down

  unsigned char* temp = (unsigned char*)malloc(width * bytesPerPixel);
  for(int s = 0; s < swaps; s++){

    unsigned char * p1 = Get(0, s);
    unsigned char * p2 = Get(0, height - (s+1));

    memmove(temp,p2,bytesPerPixel * width);
    memmove(p2,p1,bytesPerPixel * width);
    memmove(p1,temp,bytesPerPixel * width);
  }

  free(temp);

}
