#include "Camera.h"

Camera::Camera (mat4 pos) : pos(pos)
{
	inv = glm::inverse(pos);
 	R_x = mat3();
	R_y = mat3();
}

void Camera::move (mat4 movement)
{
	//pos += R_y*R_x*movement;
	pos += movement;
	inv = glm::inverse(pos);
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

vec4 Camera::transform(vec4 p)
{
	vec4 v =  pos * p;
	return p * pos;
}

vec4 Camera::itransform(vec4 p){
	return  p * inv;
}

ostream& operator<<(ostream& os, const Camera& cam)
{
	os << "(" << cam.pos[1][3] << ")";
	return os;
}
