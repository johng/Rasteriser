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
	this->depth = 2000;
}

float edgeFunction(const vec3 &a, const vec3 &b, const vec3 &p)
{ return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);}




void Rasteriser::VertexShader( const vec3& v, vec3& p_raster , Lighting lighting) {


	vec3 p_camera = lighting.transform(vec4(v,1));
	vec2 p_screen;
	float nearClippingPlane = height ;
	p_screen.x = nearClippingPlane * p_camera.x / p_camera.z;
	p_screen.y = nearClippingPlane * p_camera.y / p_camera.z;

	float l = -width / 2;
	float r = width / 2;
	float t = height / 2;
	float b = -height / 2;

	vec2 p_ndc;
	p_ndc.x = 2 * p_screen.x / (r - l) - (r + l) / (r - l);
	p_ndc.y = 2 * p_screen.y / (t - b) - (t + b) / (t - b);


	p_raster.x = (p_ndc.x + 1) / 2 * width;
	p_raster.y = (1-p_ndc.y) / 2 * height;
	p_raster.z = p_camera.z;


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

float lamdaCalc(const ivec2 &b, const ivec2 &a, const ivec2 &p)
{
	return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}


void Rasteriser::LookAt(vec3 eye, vec3 center, vec3 up){

	glm::vec3 z =  normalize(eye - center);
	vec3 x = normalize(cross(up,z));
	vec3 y = normalize(cross(z,x));
	mat4 matv = mat4();
	mat4 tr = mat4();
	for (int i=0; i<3; i++) {
		matv[0][i] = x[i];
		matv[1][i] = y[i];
		matv[2][i] = z[i];
		tr[i][3] = -center[i];
	}
	modelView = matv * tr;
}


void Rasteriser::Projection(float c){
	projection[3][2] = c;
}

void Rasteriser::ViewPort(int x, int y, int w, int h){

	viewPort[0][3] = x+w/2.f;
	viewPort[1][3] = y+h/2.f;
	viewPort[2][3] = depth/2.f;

	viewPort[0][0] = w/2.f;
	viewPort[1][1] = h/2.f;
	viewPort[2][2] = depth/2.f;
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

          if(draw_screen) {





						continue;

						vector<vec3> vertexPixels(3);

						VertexShader(t.v0, vertexPixels[0], lighting);
						VertexShader(t.v1, vertexPixels[1], lighting);
						VertexShader(t.v2, vertexPixels[2], lighting);
						//cout << vertexPixels[0].x << "," << vertexPixels[0].y << "," << vertexPixels[0].z << "," << "\n";
						//cout << vertexPixels[1].x << "," << vertexPixels[1].y << "," << vertexPixels[1].z << "," << "\n";

						//cout << xxx << "," << yyy << "\n";
						//cout << depthBufferLight[C(xxx,yyy,width,height)] << "\n";

						vec3 V0 = vertexPixels[0];
						vec3 V1 = vertexPixels[1];
						vec3 V2 = vertexPixels[2];


						vec2 p(x, y);

						float lamda0 = lamdaCalc(V1, V2, p);
						float lamda1 = lamdaCalc(V2, V0, p);
						float lamda2 = lamdaCalc(V0, V1, p);

						float totalArea = lamdaCalc(V0, V1, V2);

						lamda0 /= totalArea;
						lamda1 /= totalArea;
						lamda2 /= totalArea;

						//cout << lamda0 << "," << lamda1 << "," << lamda2 <<"," << lamda0 + lamda1 + lamda2 <<  "|" << E.x << "," << E.y << "," << E.z << "\n";

						float xx =  vertexPixels[0].x * lamda0 + vertexPixels[1].x * lamda1 + vertexPixels[2].x * lamda2;
						float yy =  vertexPixels[0].y * lamda0 + vertexPixels[1].y * lamda1 + vertexPixels[2].y * lamda2;
						float zz =  vertexPixels[0].z * lamda0 + vertexPixels[1].z * lamda1 + vertexPixels[2].z * lamda2;

						vec4 screen_coord (xx,yy,zz,1);

						vec3 light_space = lighting.transform(camera.itransform(screen_coord));


						int xxx = light_space.x/light_space.z;
						int yyy = light_space.y/light_space.z;

						//cout << xxx << "," << yyy << "\n";

						if (xxx >= 0 && xxx < width && yyy >= 0 && yyy < height) {

							//float z = 1 / (1 / V0.z * lamda0 + 1 / V1.z * lamda1 + 1 / V2.z * lamda2);

							float diff =  light_space.z - depthBufferLight[C(xxx, yyy, width, height)];

							if( diff > 0.3) {

								//cout <<   xxx <<  "," << yyy << "| " << depthBufferLight[C(xxx, yyy, width, height)] << "|" << zz << "\n";
								PutPixelSDL( screen, x, y, vec3(0,0,0) );

							}else{

								PutPixelSDL( screen, x, y, colour );
							}

						}else{
							PutPixelSDL( screen, x, y, colour );
						}


            mat3 M_i = glm::inverse(M);

            vec3 w = vec3(1, 1, 1) * M_i;

						z_buffer[C(x, y, width, height)] = W;


          }else{

            z_buffer[C(x, y, width, height)] = W;

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

	for( int i=0; i<triangles.size() ; ++i )
	{
    DrawPolygon( triangles[i], camera,lighting , depthBufferLight , false );
	}

	for(int i = 0 ; i <triangles.size(); i++){

    DrawPolygon( triangles[i], camera,lighting , depthBufferCamera , true );
	}



	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}


