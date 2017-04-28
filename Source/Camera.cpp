#include "Camera.h"

Camera::Camera (vec3 pos) : pos(pos)
{

}


vec3 Camera::position()
{
	return pos;
}

vec3 Camera::direction() {
	return dir;
}

void Camera::moveDir(vec3 pos) {
	dir += pos;
}

void Camera::movePos(vec3 m) {
	pos += m;
}


ostream& operator<<(ostream& os, const Camera& cam)
{
	os << "(" << cam.pos.x << "," << cam.pos.y << "," << cam.pos.z << ") || " << "(" << cam.dir.x << "," << cam.dir.y << "," << cam.dir.z ;
	return os;
}
