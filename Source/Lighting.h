//
// Created by John Griffith on 20/02/2017.
//

#ifndef COMPUTER_GRAPHICS_LIGHTING_H
#define COMPUTER_GRAPHICS_LIGHTING_H
#include <iostream>
#include <glm/glm.hpp>
using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;
class Lighting {


private:
		vec3 pos;
		vec3 col;
public:
		Lighting(vec3 pos, vec3 col);
		void move(vec3 m);
		void set(vec3 m);
		vec3 position();
		vec3 colour();
		friend ostream& operator<<(ostream& os, const Lighting& light);
};



#endif //COMPUTER_GRAPHICS_LIGHTING_H
