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
	this->colourBuffer = (vec3*)malloc(sizeof(vec3)*height*width);

}

float edgeFunction(const vec3 &b, const vec3 &a, const vec3 &c)
{ return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);}


void Rasteriser::DrawPolygon(Camera &camera, Lighting & lighting ,const Triangle &t) {
	//Transform to camera coordinates
	vec3 v0_dash = camera.transform(vec4(t.v0,1));
	vec3 v1_dash = camera.transform(vec4(t.v1,1));
	vec3 v2_dash = camera.transform(vec4(t.v2,1));



	cout << v0_dash.x <<"," << v0_dash.y <<"," << v0_dash.z <<"," << "\n";
	cout << v1_dash.x <<"," << v1_dash.y <<"," << v1_dash.z <<"," << "\n";
	cout << v2_dash.x <<"," << v2_dash.y <<"," << v2_dash.z <<"," << "\n";

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


				vec3 a(p.x ,- p.y, 1	);


				vec3 v0_d1 ( -v0_dash.x/v0_dash.z , -v0_dash.y/v0_dash.z, -v0_dash.z);
				vec3 v1_d1 ( -v1_dash.x/v1_dash.z , -v1_dash.y/v1_dash.z, -v1_dash.z);
				vec3 v2_d1 ( -v2_dash.x/v2_dash.z , -v2_dash.y/v2_dash.z, -v2_dash.z);



				//cout << a.x << "," << a.y << "," << 0 << "\n";
				float w0 = edgeFunction(v1_d1, v2_d1, a);
				float w1 = edgeFunction(v2_d1, v0_d1, a);
				float w2 = edgeFunction(v0_d1, v1_d1, a);

				float area = edgeFunction(v0_d1, v1_d1,v2_d1);
				//out << w0 << "," << w1 << "," << w2 << "\n";
				if (w0 >= 0 && w1 >= 0 && w2 >= 0) {

					w0 /= area;
					w1 /= area;
					w2 /= area;
					if (w0 >= 0.5 && w1 >= 0 && w2 >= 0) {


						cout << w0 << "," << w1 << "," << w2 << "\n";
						vec3 oneOverZ = v0_dash * w0 + v1_dash * w1 + v2_dash * w2;
						//float z = 1 / oneOverZ;
						cout << oneOverZ.x << "," << oneOverZ.y << "," <<  oneOverZ.z << W << "\n";
						cout << v0_dash.x <<"," << v0_dash.y <<"," << v0_dash.z <<"," << "\n";
						cout << v1_dash.x <<"," << v1_dash.y <<"," << v1_dash.z <<"," << "\n";
						cout << v2_dash.x <<"," << v2_dash.y <<"," << v2_dash.z <<"," << "\n";

					}



				}

				if (depthBufferCamera[C(x, y, width, height)] > W) {
					depthBufferCamera[C(x, y, width, height)] = W;
					colourBuffer[C(x, y, width, height)] = t.color;
					//cout << x << "," << y << "\n";
				}
			}
		}
	}
}


void Rasteriser::FillLightBuffer(Lighting & lighting, const Triangle &t){

	vec3 v0_dash_c = lighting.transform(vec4(t.v0,1));
	vec3 v1_dash_c = lighting.transform(vec4(t.v1,1));
	vec3 v2_dash_c = lighting.transform(vec4(t.v2,1));

	//Matrix of vertices
	mat3 M_l(v0_dash_c, v1_dash_c, v2_dash_c);

	mat3 M_i_l = glm::inverse(M_l);

	vec3 w_l = vec3(1, 1, 1) * M_i_l;
	//Get edge functions (rows of M_inv)

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			//vec3 p = getPoint(x, y);
			vec3 p = getPoint(x, y, width, height);
			vec3 E = M_i_l * p;
			//Check all edge functions
			if (E.x >= 0 &&
					E.y >= 0 &&
					E.z >= 0) {
				float W = 1 / glm::dot(w_l, p);
				if (depthBufferLight[C(x, y, width, height)] > W) {
					depthBufferLight[C(x, y, width, height)] = W;
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
		colourBuffer[i] = vec3(0,0,0);
	}


	for( int i=0; i<1; ++i )
	{
		FillLightBuffer(lighting, triangles[i]);
	}

	for(int i = 0 ; i < 1; i++){
		DrawPolygon(camera,lighting , triangles[i]);
	}

	for(int y = 0; y < height ; y++){
		for(int x = 0; x < width; x++){
			//if(depthBufferLight[C(x,y,width, height)] < depthBufferCamera[C(x,y,width, height)]){
				//colourBuffer[C(x,y,width, height)] = vec3(0,0,0);
			//}
			PutPixelSDL(screen, x, y, colourBuffer[C(x,y,width, height)]);
		}
	}


	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}


