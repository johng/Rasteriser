//
// Created by John Griffith on 24/03/2017.
//

#include "TGAImport.h"
#include<iostream>
#include<fstream>


using namespace std;

/*
 * HEADER
 * ID INFO
 *
 *
 */



TGAPixel::TGAPixel( char * ptr, char size){
  this->ptr = ptr;
  this->size = size;
}


bool TGAImport::ReadTGAImage(char *filename) {


  ifstream stream;
  stream.open(filename, ios::binary);

  struct _TgaHeader tgaHeader;
  stream.read((char*)&tgaHeader, sizeof(tgaHeader));


  width = tgaHeader.Width;
  height = tgaHeader.Height;
  bytesPerPixel = (int)tgaHeader.PixelDepth>>3;
  int imageType = (int)tgaHeader.ImageType;
  //todo check for valid ptr per pixel


  //ImageType
  //1,2,3 no compression, we can just read in the ptr

  int bytes = bytesPerPixel * width * height;

  texture_data = (char*)malloc(bytes);

  if(imageType == 1||imageType==2||imageType==3) {
    stream.read(texture_data, bytes);
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
          pixelInfo[b] = (char) stream.get();
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
          for(int c = 0 ; c < bytesPerPixel; c++){
            texture_data[bytePtr++] = (char) stream.get();
          }
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

    Mirror_vertically();
  }
  if (tgaHeader.ImageDescriptor & 0x10) {
    Mirror_horizontally();
  }

  stream.close();
  return true;

}

int TGAImport::GetHeight(){
  return height;
};

int TGAImport::GetWidth(){
  return width;
};


TGAPixel TGAImport::Get(int x, int y) {

  if(x < 0 || x >= width || y < 0 || y >=0){
    cout << "Invalid pixel coordinate!" << endl;
    return NULL;
  }
  char * ptr = &texture_data[x + y * width];
  TGAPixel ret(ptr, bytesPerPixel) ;
  return ret;
}


void TGAImport::Mirror_horizontally() {

  int swaps = width>>1; //Divide by 2, ensuring we round down
  for(int h =0; h< height; h++) {
    for (int s = 0; s < swaps; s++) {

      TGAPixel p1 = Get(s, h);
      TGAPixel p2 = Get(width - (s+1), h);

      //todo should probably fine a better way of doing this
      char temp[4];
      memcpy(temp,p1.ptr,p1.size);
      memcpy(p1.ptr,p2.ptr,p1.size);
      memcpy(p2.ptr,p1.ptr,p1.size);

    }
  }

}


void TGAImport::Mirror_vertically() {

  int swaps = width>>1; //Divide by 2, ensuring we round down
  for(int s = 0; s < swaps; s++){



  }




}