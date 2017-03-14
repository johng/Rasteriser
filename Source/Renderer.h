#ifndef RENDERER_H
#define RENDERER_H

#include "TestModel.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Lighting.h"

using glm::vec4;
using glm::mat4;

class Renderer {

public:

		struct Shader {
				virtual ~Shader();
				virtual vec4 proj(int triangle_index, int vertex_index) = 0;
				virtual bool fragment(vec3 bar, vec3 & colour) =0;
		};

		Renderer(SDL_Surface* screen);
		virtual void Draw(Camera &camera,Lighting &lighting,vector<Triangle>& triangles) = 0;

protected:
    SDL_Surface * screen;
		static mat4 modelView;
		static mat4 viewPort;
		static mat4 projection;
		static float depth;
		int width;
		int height;

};


#endif //COMPUTER_GRAPHICS_DRAW_H
