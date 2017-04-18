#include "Polygon.h"


Polygon::Polygon(glm::ivec3 v0,glm::ivec3 v1,glm::ivec3 v2){
  vertices[0] = v0;
  vertices[1] = v1;
  vertices[2] = v2;
}


Polygon::Polygon(glm::ivec3 v0,glm::ivec3 v1,glm::ivec3 v2, glm::ivec3 v3){
  vertices[0] = v0;
  vertices[1] = v1;
  vertices[2] = v2;
  vertices[3] = v3;
}


