#include <iostream>
#include <glm/glm.hpp>
#include "Triangle.h"
#include "Camera.h"
#include "Lighting.h"
#include "Rasteriser.h"
#define PI 3.14159265359
#define C(x,y,width,height)  (x + y * width)

vec3 Rasteriser::getPoint(int x, int y)
{
  return vec3(
          (x - width/2)/ (float) width,
          -(y - height/2)/ (float) height,
           1);
}


vec4 Rasteriser::DepthShader::proj(int triangle_index, int index) {
  vec4 vertex = vec4(r->triangles[triangle_index].vertecies[index],1);
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


vec4 Rasteriser::Shadow::proj(int triangle_index, int index) {

  vec4 vertex = vec4(r->triangles[triangle_index].vertecies[index],1);
  vec4 retval = vertex * modelView * projection * viewPort;
  for(int i = 0; i < 3 ; i ++){
    tri[i][index] = retval[i]/retval.w;
  }
  t_index = triangle_index;
  return retval;
}

bool Rasteriser::Shadow::fragment(vec3 bar, vec3 & colour) {
  vec4 p =   vec4(bar*tri,1) * screen_shadow ;
  p = p/p.w;
  int idx =  int(p[0]) + int(p[1])*r->width;

  vec3 vv = r->triangles[t_index].vertecies[0] * bar.x +
            r->triangles[t_index].vertecies[1] * bar.y +
            r->triangles[t_index].vertecies[2] * bar.z ;


  Triangle t = r->triangles[t_index];
  float diff = (float)glm::dot(t.normal,  (vv-r->light_pos));
  float light = 0.1f + std::max(0.0f,  diff)  ; //todo reduce intensity for far away


  if(idx >= 0 && idx < r->width*r->height) {
    float shadow = 0.5f + 0.7f * (r->depthBufferLight[idx] < p[2] + 44);

    float dist = length(vv- (r->light_pos) );
    dist  = dist * dist;

    vec3 intensity = r->light_colour / (float)(4 * PI * dist);
    colour = intensity * std::min<float>(shadow, 1) * t.color * light ;

	}else{
    colour = vec3(0,0,0);
  }

  return true;
}


Rasteriser::Rasteriser(SDL_Surface *screen,vector<Triangle>& triangles) : Renderer(screen), triangles(triangles) {
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

void Rasteriser::DrawPolygon(vec4 vetex[3], Shader& shader , float * z_buffer, bool draw_screen) {

  vec4 v0 = vetex[0];
  vec4 v1 = vetex[1];
  vec4 v2 = vetex[2];

  mat4 mmm = inverse(modelView * projection * viewPort) * projection * viewPort;

  vec4 v0_dash = vetex[0] * mmm;
  vec4 v1_dash = vetex[1] * mmm;
  vec4 v2_dash = vetex[2] *  mmm;


  mat3 M(v0_dash,v2_dash , v1_dash);

  mat3 M_i = glm::inverse(M);

  vec3 ww = vec3(1, 1, 1) * M_i;

  //todo bounding box
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {

      vec3 p = getPoint(x, y);
      //vec3 p(x,y,1);
      vec3 E = M_i * p;

      if (E.x < 0 ||
          E.y < 0 ||
          E.z < 0) {
        //continue;
      }

      vec3 c = barycentric(vec2(v0.x/v0.w,v0.y/v0.w),
                           vec2(v1.x/v1.w,v1.y/v1.w),
                           vec2(v2.x/v2.w,v2.y/v2.w),
                           vec2(x,y));
      float z = v0.z*c.x + v1.z*c.y + v2.z*c.z;
      float w = v0.w*c.x + v1.w*c.y + v2.w*c.z;
      int frag_depth = z/w;

      if (c.x<0 || c.y<0 || c.z<0 || z_buffer[x+y*width]>frag_depth) continue;
      vec3 colour ;
      shader.fragment(c, colour);
      z_buffer[x+y*width] = frag_depth;
      if(draw_screen)PutPixelSDL( screen, x, y, colour );
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

void Rasteriser::Draw(Camera &camera,Lighting &lighting)
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
  light_pos = lighting.position();
  camera_pos = camera.position();
  light_colour = lighting.colour();
  ViewPort(width/8, height/8, width*3/4, height*3/4);


  light_pos = normalize(light_pos);
  LookAt(light_pos, center, up);


  Projection(0);
  mat4 MM = modelView * projection * viewPort;

  DepthShader depthShader(this);

  vec4 verticies[3];

	//todo split into tiles

  for(int i = 0 ; i <triangles.size(); i++){

    for(int j = 0; j < 3 ;j++){
      verticies[j] = depthShader.proj(i,j);
    }
    DrawPolygon( verticies, depthShader  , depthBufferLight , false );
	}

  LookAt(camera_pos, center, up);
  Projection(-1.f/ length(camera_pos-center));

  mat4 camera_light =  inverse(modelView * projection * viewPort) * MM;

  Shadow shadowShader(this, camera_light, modelView);

  for(int i = 0 ; i <triangles.size(); i++){
    for(int j = 0; j < 3 ;j++){
      verticies[j] = shadowShader.proj(i,j);
    }
    DrawPolygon( verticies, shadowShader,depthBufferCamera , true );
  }

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }

  SDL_UpdateRect( screen, 0, 0, 0, 0 );
  return;
}



