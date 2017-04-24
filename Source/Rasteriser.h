#ifndef RASTERISER_H
#define RASTERISER_H

#include "Renderer.h"
#include "Lighting.h"
#include "Polygon.h"
#include "SDLauxiliary.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <vector>

//todo remove using from header file
using namespace glm;


class Rasteriser : Renderer {


public:
		Rasteriser(SDL_Surface *screen, Model * model,Camera &camera,Lighting &lighting);
		void Draw();

protected:
    vec3 light_pos;
    vec3 camera_pos;
    vec3 light_colour;
    Model * model;
		Camera &camera;
		Lighting &lighting;


private:

		float * depthBufferCamera;
		float * depthBufferLight;
		void DrawTriangle(RenderData *data, Shader &shader, float *z_buffer, Polygon *triangle, bool draw_screen);
		void DrawPolygon(vec4 *inVertices, vec2 *inTextures, int polyEdgeCount, Shader &shader, float *z_buffer, Polygon *triangle,
                         bool draw_screen);
		void LookAt(vec3 eye, vec3 center, vec3 up);
		void ViewPort(int x, int y, int w, int h);
    void Projection(float c);
    vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P);
		//void Clip(vec4 *inVerticies, int inCount);
    
		struct DepthShader: Shader {
				Rasteriser *r;
				mat3 tri;
				DepthShader(Rasteriser *rr) : r(rr),tri(0) {}
				bool colour(glm::vec3 bar, glm::vec3 &colour, Polygon *triangle, RenderData *data);
		};

		struct Shadow: Shader {
				Rasteriser * r;
				mat4 screen_shadow;
				Shadow(Rasteriser * rr, mat4 sc) : r(rr) , screen_shadow(sc) {};
				bool colour(glm::vec3 bar, glm::vec3 &colour, Polygon *triangle, RenderData *data);
		};



};


#endif
