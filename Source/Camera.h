#ifndef COMPUTER_GRAPHICS_CAMERA_H
#define COMPUTER_GRAPHICS_CAMERA_H


#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::mat4;
using glm::vec4;

class Camera
{
public:
    mat4 pos;
    mat4 inv;
    mat3 R_x, R_y;

    Camera (mat4 pos);

    void move (mat4 move);
    void rotate (float, float);

    vec4 transform(vec4);
    vec4 itransform(vec4);
    friend ostream& operator<<(ostream& os, const Camera& cam);
};




#endif
