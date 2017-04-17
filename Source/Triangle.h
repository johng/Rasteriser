#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H


#include <glm/glm.hpp>


class Triangle
{
public:
  glm::ivec3 vertices[3];
  Triangle( glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2);
  int material;
};

#endif