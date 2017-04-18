#ifndef RENDERER_H
#define RENDERER_H

#include "Polygon.h"
#include "SDLauxiliary.h"
#include "Camera.h"
#include "Lighting.h"

using glm::vec4;
using glm::mat4;

class Renderer {

public:

		struct Shader {
				virtual ~Shader();
				virtual bool colour(glm::vec3 bar, glm::vec3 &colour, Polygon *triangle) =0;
		};

		Renderer(SDL_Surface* screen);
		virtual void Draw() = 0;

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
