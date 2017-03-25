#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H


#include <glm/glm.hpp>


class Triangle
{
public:
  glm::vec3 vertices[3];
  Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
};

#endif