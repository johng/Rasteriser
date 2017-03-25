//Credit  mduffin95

#include "ObjImporter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/string_cast.hpp>

void ImportFromFile(std::string filename, std::vector<Triangle>& triangles, Material *material)
{
  std::vector<glm::vec3> vs;
  std::vector<glm::vec3> vts;
  std::vector<glm::vec3> vns;

  std::ifstream ifs(filename);
  std::string line;
  while (std::getline(ifs, line))
  {
    std::istringstream ss(line.c_str());
    ss.clear();
    ss.ignore();

    if (line.compare(0, 2, "v ") == 0) //This is a vertex
    {
      glm::vec3 v;
      for(int i=0; i<3; i++) ss >> v[i];
      vs.push_back(v);
    }
    if (line.compare(0, 3, "vt ") == 0) //This is a texture coordinate 
    {
      glm::vec3 vt;
      for(int i=0; i<3; i++) ss >> vt[i];
      vts.push_back(vt);
    }
    if (line.compare(0, 3, "vn ") == 0) //This is a vertex normal
    {
      glm::vec3 vn;
      for(int i=0; i<3; i++) ss >> vn[i];
      vts.push_back(vn);
    }
    if (line.compare(0, 2, "f ") == 0) //something else
    {
      int tmp[9];
      int i = 0;
      char discard;
      std::string token;
      while(std::getline(ss, token, ' '))
      {
        if (token == "") continue; //Bit of a hack
        std::cout << token << std::endl;
        std::string value;
        int j=0;
        std::istringstream stoken(token);
        while(std::getline(stoken, value, '/'))
        {
          std::cout << value << std::endl;
          std::istringstream svalue(value);
          svalue >> tmp[j*3+i];
          std::cout << tmp[j*3+i] << std::endl;
          j++;
        }
        i++;
      }
      Triangle tri(vs[tmp[0]-1], vs[tmp[1]-1], vs[tmp[2]-1], material);
      triangles.push_back(tri);
    }
  }
}