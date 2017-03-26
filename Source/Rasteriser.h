#ifndef RASTERISER_H
#define RASTERISER_H

#include "Renderer.h"
#include "Lighting.h"
#include "Triangle.h"
#include "SDLauxiliary.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <vector>

//todo remove using from header file
using namespace glm;


class Rasteriser : Renderer {


public:
		Rasteriser(SDL_Surface *screen, Model * model);
		void Draw(Camera &camera,Lighting &lighting);

protected:
    vec3 light_pos;
    vec3 camera_pos;
    vec3 light_colour;
    Model * model;

private:

		float * depthBufferCamera;
		float * depthBufferLight;

		vec3 getPoint(int x, int y);
		void DrawPolygon(vec4 vetex[3], Shader& shader , float * z_buffer, bool draw_screen);
		void LookAt(vec3 eye, vec3 center, vec3 up);
		void ViewPort(int x, int y, int w, int h);
    void Projection(float c);
    vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P);

		struct DepthShader: Shader {
				Rasteriser *r;
				mat3 tri;
				DepthShader(Rasteriser *rr) : r(rr),tri(0) {}
				vec4 proj(int triangle_index, int j);
				bool fragment(vec3 bar, vec3 & colour);
		};

		struct Shadow: Shader {
				Rasteriser * r;
				glm::mat3x2 textureCoordinates;
				mat4 screen_shadow;
        mat4 modelView;
				mat3 tri;
				int t_index;
				Shadow(Rasteriser * rr, mat4 sc, mat4 modelView) : r(rr) , screen_shadow(sc), modelView(modelView),tri(0) {};
				vec4 proj(int triangle_index, int vertex_index);
				bool fragment(vec3 bar, vec3 & colour);
		};



};


#endif
