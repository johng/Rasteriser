//
// Created by John Griffith on 20/02/2017.
//

#ifndef COMPUTER_GRAPHICS_LIGHTING_H
#define COMPUTER_GRAPHICS_LIGHTING_H

#include <glm/glm.hpp>
using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;
class Lighting {

public:
		Lighting(mat4 pos, vec3 colour);

		mat4 pos;
		mat4 inv;
		vec3 colour;

		void move(mat4 m);

		vec4 transform(vec4 t);

		vec4 transformi(vec4 t);
};



#endif //COMPUTER_GRAPHICS_LIGHTING_H
