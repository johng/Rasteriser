#ifndef COMPUTER_GRAPHICS_MATERIAL_H
#define COMPUTER_GRAPHICS_MATERIAL_H
#include <glm/glm.hpp>
#include "SDLauxiliary.h"
#include <unordered_map>
#include <vector>
#include <string>

struct Entry {
    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    glm::vec3 Ke;
    float Ns;
};

class Material {

private:
    std::unordered_map< std::string, int> materialNameMap;


public:
    std::vector<Entry> data;
    bool ReadMaterial(const char *filename);
    int MaterialLookup(std::string s);
};


#endif //COMPUTER_GRAPHICS_MATERIAL_H
