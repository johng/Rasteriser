#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H


#include <glm/glm.hpp>


class Polygon
{
public:
  glm::ivec3 vertices[4];
  int verticesCount;
  Polygon( glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2);
  Polygon( glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3);
  int material;
};

#endif