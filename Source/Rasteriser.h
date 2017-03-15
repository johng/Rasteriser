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
		Rasteriser(SDL_Surface *screen,vector<Triangle>& triangles);
		void Draw(Camera &camera,Lighting &lighting);

protected:

		vector<Triangle>& triangles;
    vec3 light_pos;
    vec3 camera_pos;
    vec3 light_colour;

private:

		float * depthBufferCamera;
		float * depthBufferLight;

		vec3 getPoint(int x, int y, int w, int h);
		void DrawPolygon(vec4 vetex[3], Shader& shader , float * z_buffer, bool draw_screen);
		void LookAt(vec3 eye, vec3 center, vec3 up);
		void ViewPort(int x, int y, int w, int h);
    void Projection(float c);
    vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P);

		struct DepthShader: Shader {
				Rasteriser *r;
				mat3 tri;
				DepthShader(Rasteriser *rr) : tri(0), r(rr) {}
				vec4 proj(int triangle_index, int j);
				bool fragment(vec3 bar, vec3 & colour);
		};

		struct Shadow: Shader {
				Rasteriser * r;
				mat4 screen_shadow;
        mat4 modelView;
				mat3 tri;
				int t_index;
				Shadow(Rasteriser * rr, mat4 sc, mat4 modelView) : tri(0), r(rr) , screen_shadow(sc), modelView(modelView) {};
				vec4 proj(int triangle_index, int vertex_index);
				bool fragment(vec3 bar, vec3 & colour);
		};



};


#endif
