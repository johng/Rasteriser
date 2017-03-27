#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

#include <glm/glm.hpp>
#include "Triangle.h"
#include "Texture.h"
#include <vector>

class Model {

private:
    Texture diffuse;
    Texture normal;
    Texture specular;
    std::vector<Triangle> triangles;
    std::vector<glm::vec3> vns;
    std::vector<glm::vec2> vts;
    std::vector<glm::vec3> vs;

public:
    bool LoadObj(const char * filename);
    bool LoadDiffuseTexture ( const char * filename);
    bool LoadSpecularTexture ( const char * filename);
    bool LoadNormalMap(const char * filename);

    int triangleCount();
    glm::vec3 vertex(int triangle, int index);
    glm::vec3 normalCoodinate(int triangle, int index);
    glm::vec2 textureCoordinate(int triangle, int index);

    TexturePixel diffuseTexture(glm::vec2 textureCoordinate);
    glm::vec3 normalMapTexture(glm::vec2 textureCoordinate);
    float specularTexture(glm::vec2 textureCoordinate);
};


#endif