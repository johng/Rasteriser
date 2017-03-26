
#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace glm;

bool Model::LoadObj(const char * filename)
{

  bool debug = false;

  ifstream ifs(filename);
  string line;
  while (getline(ifs, line))
  {
    istringstream ss(line.c_str());
    ss.clear();
    ss.ignore();

    if (line.compare(0, 2, "v ") == 0) //This is a vertex
    {
      vec3 v;
      for(int i=0; i<3; i++) ss >> v[i];
      vs.push_back(v);
    }
    if (line.compare(0, 3, "vt ") == 0) //This is a texture coordinate
    {
      vec2 vt;
      for(int i=0; i<2; i++) ss >> vt[i];
      vts.push_back(vt);
    }
    if (line.compare(0, 3, "vn ") == 0) //This is a vertex normal
    {
      vec3 vn;
      for(int i=0; i<3; i++) ss >> vn[i];
      vns.push_back(vn);
    }
    if (line.compare(0, 2, "f ") == 0) //something else
    {

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
  diffuseTexture = texture;

}

vec3 Model::vertex(int triangle, int index) {
  int vert_index = triangles[triangle].vertices[index].x;
  return vs[vert_index];
}

vec2 Model::textureCoordinate(int trianlge, int index){
  int texture_index = triangles[trianlge].vertices[index].y;
  return vts[texture_index];
}

int Model::triangleCount() {
  return triangles.size();
}

TexturePixel Model::diffuse(vec2 textureCoordinate) {
  ivec2 coordinate(textureCoordinate.x * diffuseTexture.GetWidth(), textureCoordinate.y * diffuseTexture.GetHeight());
  return diffuseTexture.Get(coordinate[0],coordinate[1]);
}
