//
// Created by John Griffith on 24/03/2017.
//

#include "Texture.h"
#include<iostream>
#include<fstream>

using namespace std;

TexturePixel::TexturePixel( unsigned char * ptr, char size){
  this->ptr = ptr;
  this->size = size;
}


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
        char pixelInfo[4];

        for (int b = 0; b < bytesPerPixel; b++) {
          pixelInfo[bytesPerPixel-b-1] = (char) stream.get();
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
          char pixelInfo[4];

          for (int b = 0; b < bytesPerPixel; b++) {
            pixelInfo[bytesPerPixel-b-1] = (char) stream.get();
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


TexturePixel Texture::Get(int x, int y) {

  if(x < 0 || x >= width || y < 0 || y >=height){
    cout << "Invalid pixel coordinate!" << endl;
    //todo check if quiting is too extreme
    exit(-1);
  }
  unsigned char * ptr = &texture_data[bytesPerPixel * (x + y * width)];
  TexturePixel ret(ptr, bytesPerPixel) ;
  return ret;
}


void Texture::Mirror_horizontally() {

  int swaps = width>>1; //Divide by 2, ensuring we round down
  char* temp = (char*)malloc(bytesPerPixel);

  for(int h =0; h< height; h++) {
    for (int s = 0; s < swaps; s++) {

      TexturePixel p1 = Get(s, h);
      TexturePixel p2 = Get(width - (s+1), h);

      memcpy(temp,p1.ptr,bytesPerPixel);
      memcpy(p1.ptr,p2.ptr,bytesPerPixel);
      memcpy(p2.ptr,p1.ptr,bytesPerPixel);

    }
  }

  free(temp);

}


void Texture::Mirror_vertically() {

  int swaps = height>>1; //Divide by 2, ensuring we round down

  char* temp = (char*)malloc(width * bytesPerPixel);
  for(int s = 0; s < swaps; s++){

    TexturePixel p1 = Get(0, s);
    TexturePixel p2 = Get(0, height - (s+1));

    memmove(temp,p2.ptr,bytesPerPixel * width);
    memmove(p2.ptr,p1.ptr,bytesPerPixel * width);
    memmove(p1.ptr,temp,bytesPerPixel * width);
    int c = 2;
  }

  free(temp);

}