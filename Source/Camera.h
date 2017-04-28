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
		vec3 dir;
public:

    Camera (vec3 pos,vec3 dir);
		void setPos(vec3 pos);
		void setDir(vec3 pos);

		void moveDir(vec3 pos);
		void movePos(vec3 pos);

    vec3 position();
		vec3 direction();
    friend ostream& operator<<(ostream& os, const Camera& cam);
};



#endif
