#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H
#include <string>
#include <vector>
#include "Triangle.h"
#include "Material.h"

void ImportFromFile(std::string data, std::vector<Triangle>& triangles, Material *material);


#endif