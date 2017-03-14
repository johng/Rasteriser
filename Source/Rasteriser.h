#ifndef RASTERISER_H
#define RASTERISER_H

#include "Renderer.h"
#include "Lighting.h"
#include "TestModel.h"
#include "SDLauxiliary.h"
#include <glm/glm.hpp>


using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::vec2;
using glm::mat4;
using glm::mat3;


class Rasteriser : Renderer {


public:
		Rasteriser(SDL_Surface *screen);
		void Draw(Camera &camera,Lighting &lighting,vector<Triangle>& triangles);

protected:

		int width;
		int height;

		float * depthBufferCamera;
		float * depthBufferLight;

private:
		SDL_Surface* screen;
		vec3 getPoint(int x, int y, int w, int h);
		void DrawPolygon(const Triangle &t, Shader& shader, Camera camera, Lighting lighting , float * z_buffer, bool draw_screen);
		void LookAt(vec3 eye, vec3 center, vec3 up);
		void ViewPort(int x, int y, int w, int h);
    void Projection(float c);
    vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P);
		struct DepthShader: Shader {
				mat3 tri;
				DepthShader() : tri(0) {}
				vec4 proj(vec4 vertex, int j);
				bool fragment(vec3 bar, vec3 & colour);
		};



};


#endif
