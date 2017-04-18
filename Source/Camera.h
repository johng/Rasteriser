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

private:
		vec3 pos;

public:
    float angle;
    mat3 R_x, R_y;

    Camera (vec3 pos);
    void angleDelta(float a);
    void move (vec3 move);
		void set(vec3 pos);
    void rotate (float, float);
    vec3 position();
    friend ostream& operator<<(ostream& os, const Camera& cam);
};



#endif
