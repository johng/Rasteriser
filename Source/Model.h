#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

#include <glm/glm.hpp>
#include "Polygon.h"
#include "Texture.h"
#include "Material.h"
#include <vector>

class Model {

private:
    Texture diffuse;
    Texture normal;
    Texture specular;
    Material material;
    std::vector<Polygon> triangles;
    std::vector<glm::vec3> vns;
    std::vector<glm::vec2> vts;
    std::vector<glm::vec3> vs;

public:

    bool loadedNormalTexture = 0;
    bool loadedSpecularTexture = 0;
    bool loadedDiffuseTexture = 0;
    bool loadedMaterialFile = 0;

    bool LoadObj(const char * filename);
    bool LoadDiffuseTexture ( const char * filename);
    bool LoadSpecularTexture ( const char * filename);
    bool LoadNormalMap(const char * filename);
    bool LoadMaterialsFile(const char * filename);

    glm::vec3 ambiantReflectance(int index);
    glm::vec3 specularReflectance(int index);
    glm::vec3 diffuseReflectance(int index);
    glm::vec3 glowReflectance(int index);

    int triangleCount();
    glm::vec3 vertex(int triangle, int index);
    glm::vec3 normalCoodinate(int triangle, int index);
    glm::vec2 textureCoordinate(int triangle, int index);

    unsigned char * diffuseTexture(glm::vec2 textureCoordinate);
    glm::vec3 normalMapTexture(glm::vec2 textureCoordinate);
    float specularTexture(glm::vec2 textureCoordinate);

    Polygon * GetTriangle(int triangle);
};


#endif