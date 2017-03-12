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





class Rasteriser : Renderer {



public:
		Rasteriser(SDL_Surface *screen);
		void Draw(Camera &camera,Lighting &lighting,vector<Triangle>& triangles);

private:
		SDL_Surface* screen;
		int width;
		int height;
		int depth;
		mat4 modelView;
		mat4 viewPort;
		mat4 projection;
		float * depthBufferCamera;
		float * depthBufferLight;
		vec3 getPoint(int x, int y, int w, int h);
		void DrawPolygon(const Triangle &t, Camera camera, Lighting lighting , float * z_buffer, bool draw_screen);
		void LookAt(vec3 eye, vec3 center, vec3 up);
		void ViewPort(int x, int y, int w, int h);
    void Projection(float c);
    vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P);
};


#endif
