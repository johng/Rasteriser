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


vec4 Rasteriser::DepthShader::proj(vec4 vertex, int index) {
  vec4 retval = vertex * modelView * projection * viewPort;
  for(int i = 0; i < 3 ; i ++){
    tri[i][index] = retval[i]/retval.w;
  }
  return retval;
}

bool Rasteriser::DepthShader::fragment(vec3 bar, vec3 & colour) {
  vec3 p = bar*tri;
  colour = vec3(1, 1, 1)*(p.z/depth);
  return true;
}


vec4 Rasteriser::Shadow::proj(vec4 vertex, int index) {
  vec4 retval = vertex * modelView * projection * viewPort;
  for(int i = 0; i < 3 ; i ++){
    tri[i][index] = retval[i]/retval.w;
  }
  return retval;
}

bool Rasteriser::Shadow::fragment(vec3 bar, vec3 & colour) {
  vec4 p =   vec4(bar*tri,1) * screen_shadow ;
  p = p/p.w;
  int idx =  int(p[0]) + int(p[1])*r->width;
  //cout << p[0] << "," << p[1] << r->width << "\n";
  //cout << idx << "\n";

  if(idx >= 0 && idx < r->width*r->height) {

    float shadow = .3 + .7 * (r->depthBufferLight[idx] < p[2] + 44);


    colour = vec3(1, 1, 1) * std::min<float>(shadow, 1);

  }else{
    colour = vec3(0,0,0);
  }



  return true;
}


Rasteriser::Rasteriser(SDL_Surface *screen) : Renderer(screen) {
  this->depth = 2000.f;
	this->screen = screen;
	this->width = screen->w;
	this->height = screen->h;
	this->depthBufferCamera = (float*)malloc(sizeof(float)*height*width);
	this->depthBufferLight = (float*)malloc(sizeof(float)*height*width);
}


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

void Rasteriser::DrawPolygon(const Triangle &t, Shader& shader ,Camera camera, Lighting lighting , float * z_buffer, bool draw_screen) {

  //Transform to camera coordinates
  vec4 vv0_dash = shader.proj(vec4(t.v0,1),0);
  vec4 vv1_dash = shader.proj(vec4(t.v1,1),1);
  vec4 vv2_dash = shader.proj(vec4(t.v2,1),2);

  //todo bounding box
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      vec3 c = barycentric(vec2(vv0_dash.x/vv0_dash.w,vv0_dash.y/vv0_dash.w),
                           vec2(vv1_dash.x/vv1_dash.w,vv1_dash.y/vv1_dash.w),
                           vec2(vv2_dash.x/vv2_dash.w,vv2_dash.y/vv2_dash.w),
                           vec2(x,y));
      float z = vv0_dash.z*c.x + vv1_dash.z*c.y + vv2_dash.z*c.z;
      float w = vv0_dash.w*c.x + vv1_dash.w*c.y + vv2_dash.w*c.z;
      int frag_depth = z/w;

      if (c.x<0 || c.y<0 || c.z<0 || z_buffer[x+y*width]>frag_depth) continue;
      vec3 colour ;
      shader.fragment(c, colour);
      z_buffer[x+y*width] = frag_depth;
      if(draw_screen)PutPixelSDL( screen, x, y, t.color * colour );
    }
  }

  //todo integrate this with the code above
  /*
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
  */
}

void Rasteriser::Projection(float coeff) {
  projection = mat4(1);
  projection[3][2] = coeff;
}

void Rasteriser::LookAt(vec3 eye, vec3 center, vec3 up){

  vec3 z = normalize(eye - center);
  vec3 x = normalize(cross(up,z));
  vec3 y = normalize(cross(z,x));

  modelView = mat4(1);
  for (int i=0; i<3; i++) {
    modelView[0][i] = x[i];
    modelView[1][i] = y[i];
    modelView[2][i] = z[i];
    modelView[i][3] = -center[i];
  }
}


void Rasteriser::ViewPort(int x, int y, int w, int h){
  viewPort = mat4(1);
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
		depthBufferCamera[i] = -INFINITY;
		depthBufferLight[i] = -INFINITY;
	}

  vec3 center(0,0,0);
  vec3 up(0,1,0);
  vec3 light = lighting.position();
  vec3 eye = camera.position();

  ViewPort(width/8, height/8, width*3/4, height*3/4);


  light = normalize(light);
  LookAt(light, center, up);


  Projection(0);
  mat4 MM = modelView * projection * viewPort;

  DepthShader depthShader(this);
	for(int i = 0 ; i <triangles.size(); i++){
    DrawPolygon( triangles[i], depthShader , camera,lighting , depthBufferLight , false );
	}


  LookAt(eye, center, up);
  Projection(-1.f/ length(eye-center));

  mat4 M =  inverse(modelView * projection * viewPort) * MM;


  Shadow shadowShader(this, M);

  for(int i = 0 ; i <triangles.size(); i++){
    DrawPolygon( triangles[i], shadowShader , camera,lighting , depthBufferCamera , true );
  }


	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}



