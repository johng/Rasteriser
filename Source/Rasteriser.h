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





class Rasteriser : Renderer {



public:
		Rasteriser(SDL_Surface *screen);
		void Draw(Camera &camera,Lighting &lighting,vector<Triangle>& triangles);

private:
		SDL_Surface* screen;
		int width;
		int height;
		float * depthBufferCamera;
		float * depthBufferLight;
		vec3 getPoint(int x, int y, int w, int h);
		void DrawPolygon(const Triangle &t, Camera camera, Lighting lighting , float * z_buffer, bool draw_screen);
		bool Fragment(vec3 bac, vec3 & colour,Camera camera, Lighting lighting);
		void VertexShader( const vec3& v, vec3& p_raster);
};


#endif
