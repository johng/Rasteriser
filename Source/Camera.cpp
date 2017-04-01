#include "Camera.h"

Camera::Camera (vec3 pos) : pos(pos)
{

}

void Camera::move (vec3 movement)
{
	//pos += R_y*R_x*movement;
	pos += movement;
	//inv = glm::inverse(pos);
}

void Camera::set(vec3 p){
	pos = p;
}

void Camera::rotate (float pitch, float yaw)
{
	R_x *= mat3(1, 0, 0,  // 1. column
							0, cos(pitch), -sin(pitch),  // 2. column
							0, sin(pitch), cos(pitch)); // 3. column

	R_y *= mat3(cos(yaw), 0, -sin(yaw),  // 1. column
							0, 1, 0,  // 2. column
							sin(yaw), 0, cos(yaw)); // 3. column
}

vec3 Camera::position()
{
	return pos;
}



ostream& operator<<(ostream& os, const Camera& cam)
{
	os << "(" << cam.pos.x << "," << cam.pos.y << "," << cam.pos.z << ")";
	return os;
}
