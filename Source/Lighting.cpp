#include "Lighting.h"


Lighting::Lighting (vec3 pos, vec3 colour) : pos(pos),col(colour)
{
}

void Lighting::move (vec3 movement)
{
  pos += movement;
}

void Lighting::set (vec3 p)
{
  pos = p;
}

vec3 Lighting::position() {
  return pos;
}


vec3 Lighting::colour() {
  return col;
}

ostream& operator<<(ostream& os, const Lighting& lighting)
{
  os << "(" << lighting.pos.x << "," << lighting.pos.y << "," << lighting.pos.z << ")";
  return os;
}
