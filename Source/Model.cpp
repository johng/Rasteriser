
#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace glm;

bool Model::LoadObj(const char * filename)
{

  bool debug = false;

  std::ifstream ifs(filename);
  std::string line;
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
      glm::vec3 vt;
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
    if (line.compare(0, 2, "f ") == 0) //something else
    {
      ss.ignore();

      int i = 0;
      char discard;

      ivec3 v[3];
      //todo handle negative indices
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
          v[i][j]--;
          j++;

        }
        if(debug)cout << endl;
        i++;
      }


      if(debug)cout << endl;
      Triangle triangle (v[0],v[1],v[2]);
      triangles.push_back(triangle);
    }
  }
}


bool Model::LoadDiffuseTexture(const char *filename) {

  Texture texture;
  texture.ReadTGAImage(filename);
  diffuse = texture;

}


bool Model::LoadNormalMap(const char * filename){
  Texture texture;
  texture.ReadTGAImage(filename);
  normal = texture;
}


bool Model::LoadSpecularTexture(const char * filename){
  Texture texture;
  texture.ReadTGAImage(filename);
  specular = texture;
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

int Model::triangleCount() {
  return triangles.size();
}

TexturePixel Model::diffuseTexture(vec2 textureCoordinate) {
  ivec2 coordinate(textureCoordinate.x * diffuse.GetWidth(), textureCoordinate.y * diffuse.GetHeight());
  return diffuse.Get(coordinate[0],coordinate[1]);
}

vec3 Model::normalMapTexture(vec2 textureCoordinate){
  ivec2 coordinate(textureCoordinate.x * normal.GetWidth(), textureCoordinate.y * normal.GetHeight());
  TexturePixel c = normal.Get(coordinate[0],coordinate[1]);
  vec3 res;
  for (int i=0; i<3; i++)
    res[2-i] = ((int)c.ptr[i])/255.f*2.f - 1.f;

  return res;
}


float Model::specularTexture(vec2 textureCoordinate){
  ivec2 coordinate(textureCoordinate.x * specular.GetWidth(), textureCoordinate.y * specular.GetHeight());
  return ((int)(specular.Get(coordinate[0],coordinate[1]).ptr[0]))/1.0f;
}


