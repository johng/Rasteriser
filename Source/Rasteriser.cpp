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

bool Rasteriser::Fragment(vec3 bac, vec3 & colour,Camera camera, Lighting lighting){


  //Shader shader(ModelView, (Projection*ModelView).invert_transpose(), M*(Viewport*Projection*ModelView).invert());
  //Shader(Matrix M, Matrix MIT, Matrix MS) : uniform_M(M), uniform_MIT(MIT), uniform_Mshadow(MS), varying_uv(), varying_tri() {}

  /*
   * mat<4,4,float> uniform_M;   //  Projection*ModelView
    mat<4,4,float> uniform_MIT; // (Projection*ModelView).invert_transpose()
    mat<4,4,float> uniform_Mshadow; // transform framebuffer screen coordinates to shadowbuffer screen coordinates
    mat<2,3,float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<3,3,float> varying_tri; // triangle coordinates before Viewport transform, written by VS, read by FS

   */

    /*

     Vec4f sb_p = uniform_Mshadow*embed<4>(varying_tri*bar); // corresponding point in the shadow buffer
        sb_p = sb_p/sb_p[3];
        int idx = int(sb_p[0]) + int(sb_p[1])*width; // index in the shadowbuffer array
        float shadow = .3+.7*(shadowbuffer[idx]<sb_p[2]);
        Vec2f uv = varying_uv*bar;                 // interpolate uv for the current pixel
        Vec3f n = proj<3>(uniform_MIT*embed<4>(model->normal(uv))).normalize(); // normal
        Vec3f l = proj<3>(uniform_M  *embed<4>(light_dir        )).normalize(); // light vector
        Vec3f r = (n*(n*l*2.f) - l).normalize();   // reflected light
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        float diff = std::max(0.f, n*l);
        TGAColor c = model->diffuse(uv);
        for (int i=0; i<3; i++) color[i] = std::min<float>(20 + c[i]*shadow*(1.2*diff + .6*spec), 255);
        return false;
    */


  mat4 framebuffer_screen_coord;





}

void Rasteriser::DrawPolygon(const Triangle &t, Camera camera, Lighting lighting , float * z_buffer, bool draw_screen) {

  //Transform to camera coordinates

  vec3	v0_dash = t.v0;
  vec3	v1_dash = t.v1;
  vec3	v2_dash = t.v2;

  if(draw_screen){

    v0_dash = camera.transform(vec4(v0_dash,1));
    v1_dash = camera.transform(vec4(v1_dash,1));
    v2_dash = camera.transform(vec4(v2_dash,1));

  }else{
    v0_dash = lighting.transform(vec4(v0_dash,1));
    v1_dash = lighting.transform(vec4(v1_dash,1));
    v2_dash = lighting.transform(vec4(v2_dash,1));

  }



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

      //cout << E.x << "," << E.y << "," << E.z << "\n";

      if (E.x >= 0 &&
          E.y >= 0 &&
          E.z >= 0) {

        float W = 1 / glm::dot(w, p);

        float w0 = E.x;
        float w1 = E.y;
        float w2 = E.z;

        vec3 colour = t.color;

        if (z_buffer[C(x, y, width, height)] > W) {

          if(draw_screen){

            vec3 v0_dash_light = lighting.transform(vec4(t.v0,1));
            vec3 v1_dash_light = lighting.transform(vec4(t.v1,1));
            vec3 v2_dash_light = lighting.transform(vec4(t.v2,1));

            mat3 M(v0_dash_light,v1_dash_light, v2_dash_light);

            vec4 bac_proj(E * M ,1);


            //cout << E.x + E.y + E.z << "\n";

            //cout << bac_proj[0] << "," << bac_proj[1] << "," <<bac_proj[2] << "," << "\n";
            //cout << v0_dash_light[0] << "," << v0_dash_light[1] << "," <<v0_dash_light[2] << "," << "\n";

            vec4 coord = bac_proj * lighting.pos;


            //cout << coord.x * width << "," << coord.y * height << "," << coord.z << "," << coord.w << "\n";

            int xx =  (int)coord[0]/ (float)coord[2];
            int yy = (int)coord[1]/ (float)coord[2];

            vec3 colour(0,0,0);

            //cout << xx << "," << yy << "\n";
            if(xx >= 0 && xx < width && yy >= 0 && yy < height) {

              float shadow = depthBufferLight[C(xx, yy, width, height)];

              //cout << shadow << "\n";

              if(shadow >= coord[2]){
                colour = t.color;
              }
            }

            mat3 M_i = glm::inverse(M);

            vec3 w = vec3(1, 1, 1) * M_i;

            PutPixelSDL( screen, x, y, colour );
            z_buffer[C(x, y, width, height)] = W;

              /*
              Vec4f sb_p = uniform_Mshadow*embed<4>(varying_tri*bar); // corresponding point in the shadow buffer
              sb_p = sb_p/sb_p[3];
              int idx = int(sb_p[0]) + int(sb_p[1])*width; // index in the shadowbuffer array
              float shadow = .3+.7*(shadowbuffer[idx]<sb_p[2]);
              Vec2f uv = varying_uv*bar;                 // interpolate uv for the current pixel
              Vec3f n = proj<3>(uniform_MIT*embed<4>(model->normal(uv))).normalize(); // normal
              Vec3f l = proj<3>(uniform_M  *embed<4>(light_dir        )).normalize(); // light vector
              Vec3f r = (n*(n*l*2.f) - l).normalize();   // reflected light
              float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
              float diff = std::max(0.f, n*l);
              TGAColor c = model->diffuse(uv);
              for (int i=0; i<3; i++) color[i] = std::min<float>(20 + c[i]*shadow*(1.2*diff + .6*spec), 255);
              return false;
              */


          }else{

            z_buffer[C(x, y, width, height)] = W;

            if(y == 416)
              cout << "Light:" << x << "," << y << "| " << W <<  "\n" ;


          }
        }
      }
    }
  }
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

	for( int i=0; i<1; ++i )
	{
    DrawPolygon( triangles[i], camera,lighting , depthBufferLight , false );
	}

	for(int i = 0 ; i < 1; i++){

    DrawPolygon( triangles[i], camera,lighting , depthBufferCamera , true );
	}



	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}


