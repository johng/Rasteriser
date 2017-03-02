#include <iostream>
#include <glm/glm.hpp>
#include "TestModel.h"
#include "Camera.h"
#include "Lighting.h"
#include "Rasteriser.h"


#define C(x,y,width,height)  (x + y * width)

vec3 Rasteriser::getPoint(int x, int y, int w, int h)
{
  return vec3(
          (x - width/2)/ (float) width,
          -(y - height/2)/ (float) height,
           1);
}


Rasteriser::Rasteriser(SDL_Surface *screen) : Renderer(screen) {
	this->screen = screen;
	this->width = screen->w;
	this->height = screen->w;
	this->depthBufferCamera = (float*)malloc(sizeof(float)*height*width);
	this->depthBufferLight = (float*)malloc(sizeof(float)*height*width);
}

float edgeFunction(const vec3 &a, const vec3 &b, const vec3 &p)
{ return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);}


void Rasteriser::VertexShader( const vec3& v, vec3& p_raster ) {

	glm::vec2 p_screen;
	float nearClippingPlane = height;
	p_screen.x = nearClippingPlane * v.x / v.z;
	p_screen.y = nearClippingPlane * v.y / v.z;

	float l = -width / 2;
	float r = width / 2;
	float t = height / 2;
	float b = -height / 2;

	glm::vec2 p_ndc;
	p_ndc.x = 2 * p_screen.x / (r - l) - (r + l) / (r - l);
	p_ndc.y = 2 * p_screen.y / (t - b) - (t + b) / (t - b);

	p_raster.x = (p_ndc.x + 1) / 2 * width;
	p_raster.y = (1-p_ndc.y) / 2 * height;

	p_raster.z = v.z;
}

void Rasteriser::DrawPolygon(Camera &camera, Lighting & lighting ,const Triangle &t, float * buffer, bool colour) {
	//Transform to camera coordinates

	vec3 v0_dash ;
	vec3 v1_dash ;
	vec3 v2_dash ;

	if(colour){
		v0_dash = camera.transform(vec4(t.v0,1));
		v1_dash = camera.transform(vec4(t.v1,1));
		v2_dash = camera.transform(vec4(t.v2,1));
	}else{
		v0_dash = lighting.transform(vec4(t.v0,1));
		v1_dash = lighting.transform(vec4(t.v1,1));
		v2_dash = lighting.transform(vec4(t.v2,1));
	}

	//Matrix of vertices
	mat3 M(v0_dash, v1_dash, v2_dash);

	mat3 M_i = glm::inverse(M);

	vec3 w = vec3(1, 1, 1) * M_i ;
	//Get edge functions (rows of M_inv)

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			vec3 p = getPoint(x, y, width, height);
			//vec3 p(x,y,1);
			vec3 E = M_i * p;
			//Check all edge functions
			if (E.x >= 0 &&
					E.y >= 0 &&
					E.z >= 0) {

				float W = 1 / glm::dot(w, p);

				float w0 ;
				float w1 ;
				float w2 ;


				w0 = E.x ;
				w1 = E.y ;
				w2 = E.z ;

				vec3 oneOverZ = v0_dash * w0 + v1_dash * w1 + v2_dash * w2;

				float z = 1/ ( w0 * 1/v0_dash.z + w1 * 1/v1_dash.z + w2 * 1/v2_dash.z  );

				if (buffer[C(x, y, width, height)] > z) {

					buffer[C(x, y, width, height)] = z;

					if(colour){
//						PutPixelSDL(screen, x, y, t.color);


						vec4 world_pos(vec4(oneOverZ.x,oneOverZ.x,z,1));

						vec4 cc = camera.itransform(world_pos);
						vec3 light_space = lighting.transform(cc);
						//vec4 camera_space = camera.transform(world_pos);

						vec3 cam_proj;
						vec3 light_proj;

						VertexShader(light_space, light_proj);



						int x_light = (int)light_proj.x;
						int y_light = (int)light_proj.y;
						cout << x_light << "," << y_light << "," << light_proj.z << "|" << x << "," << y << "\n";

						if(x_light >= 0 && x_light <= width  && y_light  >= 0 && y_light <= height){

							if(depthBufferLight[C(x_light,y_light,width,height)] <= light_proj.z ){
								//cout << depthBufferLight[C(x_light,y_light,width,height)] << ","<<light_proj.z <<"\n";
								PutPixelSDL(screen, x, y, t.color);
							}else{
								PutPixelSDL(screen, x, y, vec3(0,0,0));
							}
						}else {
								PutPixelSDL(screen, x, y, t.color);
						}
					}
				}

			}
		}
	}
}



void Rasteriser::Draw(Camera & camera, Lighting & lighting ,vector<Triangle>& triangles)
{

	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) ) {
		SDL_LockSurface(screen);
	}

	for(int i = 0 ; i < height * width ; i ++){
		depthBufferCamera[i] = INFINITY;
		depthBufferLight[i] = INFINITY;
	}

	for( int i=0; i<2; ++i )
	{
		DrawPolygon(camera,lighting , triangles[i],depthBufferLight, false);
	}
	cout << "--------\n";
	for(int i = 0 ; i < 2; i++){
		DrawPolygon(camera,lighting , triangles[i],depthBufferCamera,true);
	}

	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}


