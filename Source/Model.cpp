
#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;
using namespace glm;

bool Model::LoadObj(const char * filename)
{

  bool debug = false;
  char current_material[20];
  std::ifstream ifs(filename);
  std::string line;
  int currentMaterial = -1;
  while (std::getline(ifs, line))
  {
    std::istringstream ss(line);
    ss.clear();

    if (line.compare(0, 2, "v ") == 0) //This is a vertex
    {
      ss.ignore();
      glm::vec3 v;
      for(int i=0; i<3; i++) ss >> v[i];

      vs.push_back(v);
    }
    if (line.compare(0, 3, "vt ") == 0) //This is a texture coordinate
    {
      ss.ignore(2);
      glm::vec2 vt;
      for(int i=0; i<2; i++) ss >> vt[i];

      vts.push_back(vt);
    }
    if (line.compare(0, 3, "vn ") == 0) //This is a vertex normal
    {
      ss.ignore(2);
      glm::vec3 vn;
      for(int i=0; i<3; i++) ss >> vn[i];
      vns.push_back(vn);
    }

    if (line.compare(0,6, "usemtl") == 0)
    {
      ss.ignore(6);

      std::istream_iterator<char> it(ss);
      std::istream_iterator<char> end;
      std::string results(it, end);
      int index = material.MaterialLookup(results);
      currentMaterial = index;
      cout << results <<  ":"<<index<<endl;

    }

    if (line.compare(0, 2, "f ") == 0) //something else
    {
      ss.ignore();

      int i = 0;
      char discard;

      ivec3 v[4];
      string token;
      while(getline(ss, token, ' '))
      {
        if (token == "") continue; //Bit of a hack
        if(debug) cout << token << endl;
        string value;
        int j=0;
        istringstream stoken(token);

        while(getline(stoken, value, '/'))
        {
          istringstream svalue(value);
          svalue >> v[i][j];
          if(debug)cout << v[i][j] << ",";
          if(v[i][j] < 0){
            v[i][j] += vs.size();
          }else {

            v[i][j]--;
          }
          j++;

        }
        if(debug)cout << endl;
        i++;
      }


      if(debug)cout << endl;
      Polygon polygon (v[0],v[1],v[2],v[3]);
      if(currentMaterial >= 0){
        polygon.material = currentMaterial;
      }
      polygon.verticesCount = i;
      triangles.push_back(polygon);
    }
  }
  return true;
}

bool Model::LoadMaterialsFile(const char *filename){

  Material m;
  m.ReadMaterial(filename);
  material = m;

  return true;

}

glm::vec3 Model::ambiantReflectance(int index){
  return material.data[index].Ka;
}

glm::vec3 Model::diffuseReflectance(int index){
  return material.data[index].Kd;
}


glm::vec3 Model::specularReflectance(int index){
  return material.data[index].Ks;
}


glm::vec3 Model::glowReflectance(int index){
  return material.data[index].Ke;
}



bool Model::LoadDiffuseTexture(const char *filename) {

  Texture texture;
  texture.ReadTGAImage(filename);
  diffuse = texture;
  loadedDiffuseTexture = true;
  return true;
}


bool Model::LoadNormalMap(const char * filename){
  Texture texture;
  texture.ReadTGAImage(filename);
  normal = texture;
  loadedNormalTexture = true;
  return true;
}


bool Model::LoadSpecularTexture(const char * filename){
  Texture texture;
  texture.ReadTGAImage(filename);
  specular = texture;
  loadedNormalTexture = true;
  return true;
}


vec3 Model::vertex(int triangle, int index) {
  int vert_index = triangles[triangle].vertices[index].x;
  return vs[vert_index];
}

vec2 Model::textureCoordinate(int triangle, int index){
  int texture_index = triangles[triangle].vertices[index].y;
  return vts[texture_index];
}

vec3 Model::normalCoodinate(int triangle, int index){
  int nm_index = triangles[triangle].vertices[index].z;
  return vns[nm_index];
};

Polygon * Model::GetTriangle(int triangle){
  return &triangles[triangle];
}

int Model::triangleCount() {
  return triangles.size();
}

unsigned char * Model::diffuseTexture(vec2 textureCoordinate) {
  ivec2 coordinate(textureCoordinate.x * diffuse.GetWidth(), textureCoordinate.y * diffuse.GetHeight());
  return diffuse.Get(coordinate[0],coordinate[1]);
}

vec3 Model::normalMapTexture(vec2 textureCoordinate){
  ivec2 coordinate(textureCoordinate.x * normal.GetWidth(), textureCoordinate.y * normal.GetHeight());
  unsigned char * c = normal.Get(coordinate[0],coordinate[1]);
  vec3 res;
  for (int i=0; i<3; i++) {
    //Each texture is an ZYX colour.
    //To convert to x,y,z vector we divide by 255 for the range, [0,1]
    //We then convert to -1,1.
    res[2-i] = (c[i] / 255.0f) * 2.f - 1.f;
  }
  return res;
}


float Model::specularTexture(vec2 textureCoordinate){
  ivec2 coordinate(textureCoordinate.x * specular.GetWidth(), textureCoordinate.y * specular.GetHeight());
  return (float)(specular.Get(coordinate[0],coordinate[1])[0]);
}


