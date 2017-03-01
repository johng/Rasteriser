#include "Lighting.h"


Lighting::Lighting (glm::mat4 pos, vec3 colour) : pos(pos),colour(colour)
{
  inv = glm::inverse(pos);
}

void Lighting::move (glm::mat4 movement)
{
  pos += movement;
}

vec4 Lighting::transform(vec4 p)
{
  return p * pos;
}


vec4 Lighting::transformi(vec4 p)
{
  return p * inv;
}