
#include "Material.h"



bool Material::ReadMaterial(const char *filename) {


  //Read material
  std::string name;
  //materialNameMap[name, data.length];
}


int Material::MaterialLookup(std::string s) {
  return materialNameMap[s];
}