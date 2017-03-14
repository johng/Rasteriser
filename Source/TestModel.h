#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <glm/glm.hpp>
#include <vector>

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec3 vertecies[3];
	glm::vec3 normal;
	glm::vec3 color;
	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color );
	void ComputeNormal();
};
void LoadTestModel( std::vector<Triangle>& triangles );

#endif