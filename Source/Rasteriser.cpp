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
  this->depth = 100;
	this->screen = screen;
	this->width = screen->w;
	this->height = screen->w;
	this->depthBufferCamera = (float*)malloc(sizeof(float)*height*width);
	this->depthBufferLight = (float*)malloc(sizeof(float)*height*width);
}

float edgeFunction(const vec3 &a, const vec3 &b, const vec3 &p)
{ return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);}


vec3 Rasteriser::barycentric(vec2 A, vec2 B, vec2 C, vec2 P) {
  vec3 s[2];
  for (int i=2; i--; ) {
    s[i][0] = C[i]-A[i];
    s[i][1] = B[i]-A[i];
    s[i][2] = A[i]-P[i];
  }
  vec3 u = cross(s[0], s[1]);
  if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
    return vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
  return vec3(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}


void Rasteriser::DrawPolygon(const Triangle &t, Camera camera, Lighting lighting , float * z_buffer, bool draw_screen) {

  //Transform to camera coordinates




  //vec4 v0_dash = viewPort*projection*modelView*(vec4(t.v0,1));
  //vec4 v1_dash = viewPort*projection*modelView*(vec4(t.v1,1));
  //vec4 v2_dash = viewPort*projection*modelView*(vec4(t.v2,1));


  vec4 v0_dash = projection*modelView*(vec4(t.v0,1));
  vec4 v1_dash = projection*modelView*(vec4(t.v1,1));
  vec4 v2_dash = projection*modelView*(vec4(t.v2,1));

  cout << t.v0.x << "," << t.v0.y << "," << t.v0.z << "\n";
  cout << v0_dash.x << "," << v0_dash.y << "," << v0_dash.z << "\n";
  //cout << vv.x << "," << vv.y << "," << vv.z << "\n";

  /*
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      vec3 c = barycentric(vec2(v0_dash.x/v0_dash.z,v0_dash.y/v0_dash.z),
                           vec2(v1_dash.x/v1_dash.z,v1_dash.y/v1_dash.z),
                           vec2(v2_dash.x/v2_dash.z,v2_dash.y/v2_dash.z),
                           vec2(x,y));
      float z = v0_dash.z*c.x + v1_dash.z*c.y + v2_dash.z*c.z;
      float w = v0_dash.w*c.x + v1_dash.w*c.y + v2_dash.w*c.z;
      int frag_depth = z/w;

      if (c.x<0 || c.y<0 || c.z<0 || z_buffer[x+y*width]<frag_depth) continue;
      z_buffer[x+y*width] = frag_depth;
      PutPixelSDL( screen, x, y, t.color );
    }
  }
*/



  //Matrix of vertices
  mat3 M(v0_dash, v1_dash, v2_dash);

  mat3 M_i = glm::inverse(M);

  vec3 w = vec3(1, 1, 1) * M_i;


  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      vec3 p = getPoint(x, y, width, height);
      //vec3 p(x,y,1);
      vec3 E = M_i * p;
      //Check all edge functions

      if (E.x >= 0 &&
          E.y >= 0 &&
          E.z >= 0) {

        //cout << E.x << "," << E.y << "," << E.z << "\n";

        float W = 1 / glm::dot(w, p);

        vec3 colour = t.color;

        if (z_buffer[C(x, y, width, height)] > W) {
            PutPixelSDL( screen, x, y, colour );
            z_buffer[C(x, y, width, height)] = W;
        }
      }
    }
  }
}

void Rasteriser::Projection(float coeff) {
  projection = mat4();
  projection[3][2] = coeff;
}

void Rasteriser::LookAt(vec3 eye, vec3 center, vec3 up){

  vec3 z =  normalize(eye - center);
  vec3 x = normalize(cross(up,z));
  vec3 y = normalize(cross(z,x));
  mat4 matv(1);
  mat4 tr(1);
  for (int i=0; i<3; i++) {
    matv[0][i] = x[i];
    matv[1][i] = y[i];
    matv[2][i] = z[i];
    tr[i][3] = -center[i];
  }
  modelView = matv * tr;
}


void Rasteriser::ViewPort(int x, int y, int w, int h){
  viewPort = mat4();
  viewPort[0][3] = x+w/2.f;
  viewPort[1][3] = y+h/2.f;
  viewPort[2][3] = depth/2.f;

  viewPort[0][0] = w/2.f;
  viewPort[1][1] = h/2.f;
  viewPort[2][2] = depth/2.f;
}

void Rasteriser::Draw(Camera &camera,Lighting &lighting,vector<Triangle>& triangles)
{

	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) ) {
		SDL_LockSurface(screen);
	}

	for(int i = 0 ; i < height * width ; i ++){
		depthBufferCamera[i] = INFINITY;
		depthBufferLight[i] = INFINITY;
	}

  vec3 center(0,0,0);
  vec3 up(0,1,0);

  vec3 eye(0,0,4);
  LookAt(eye, center, up);
  ViewPort(0,0,width,height);
  float c = length(eye-center);
  cout << c << "\n";
  Projection(c);

	for( int i=0; i<1; ++i )
	{
    //DrawPolygon( triangles[i], camera,lighting , depthBufferLight , false );
	}

	for(int i = 0 ; i <triangles.size(); i++){

    DrawPolygon( triangles[i], camera,lighting , depthBufferCamera , true );
	}



	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}



