#ifndef RASTERISER_H
#define RASTERISER_H

#include "Renderer.h"
#include "Lighting.h"
#include "TestModel.h"
#include "SDLauxiliary.h"
#include <glm/glm.hpp>


using glm::vec3;
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
		vec3 * colourBuffer;
		vec3 getPoint(int x, int y, int w, int h);
		void DrawPolygon(Camera &camera,Lighting & lighting, const Triangle &t);
		void FillLightBuffer(Lighting& lighting, const Triangle &t);
};


#endif
