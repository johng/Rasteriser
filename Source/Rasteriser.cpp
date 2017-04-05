#include <iostream>
#include <glm/glm.hpp>
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

  vec4 vertex = vec4(r->model->vertex(triangle_index,index),1);
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
  textureCoordinates[index] = r->model->textureCoordinate(triangle_index,index);
  vec4 vertex = vec4(r->model->vertex(triangle_index,index),1);
  vec4 retval = vertex * modelView * projection * viewPort;
  for(int i = 0; i < 3 ; i ++){
    //Projecting the triangle into screen space
    tri[i][index] = retval[i]/retval.w;
  }
  t_index = triangle_index;
  return retval;
}
bool Rasteriser::Shadow::fragment(vec3 bar, vec3 & colour) {

  vec4 p = vec4(bar*tri,1) * screen_shadow ;
  p = p/p.w;
  int idx =  int(p[0]) + int(p[1])*r->width;
  vec2 textureCoordInterp =   textureCoordinates * bar;


  vec4 normal = vec4(r->model->normalMapTexture(textureCoordInterp),1); // normal


	mat4 mm = transpose(inverse( modelView * projection));

	vec3 vv = vec3( normal*mm);
	vec3 n =  normalize( vv);
  vec3 ll =  vec4(r->light_pos,1) * modelView ;
  vec3 l = normalize(ll);
	float ttt = glm::dot(n,l)*2.0f;
	vec3 tt = n*ttt - l;
	vec3 ref = normalize(tt);
  float spec = pow(std::max<float>(ref.z, 0.0f), r->model->specularTexture(textureCoordInterp));
  float diff = std::max<float>(0.f, glm::dot(n,l));



	unsigned char * diffuse = r->model->diffuseTexture(textureCoordInterp);

  if(idx >= 0 && idx < r->width*r->height) {

    float shadow = 0.3f + 0.7f * (r->depthBufferLight[idx] < p[2] + 20);

    //for (int i=0; i<3; i++) colour[i] =  c[i] ;
    for (int i=0; i<3; i++) colour[2-i] = std::min<float>(20.0f + diffuse[i]*shadow*( 0.6f* spec+ 1.0f*diff), 255);


    //colour = intensity * std::min<float>(shadow, 1) * cc ;
	}else{
    colour = vec3(0,0,0);
  }

  return true;
}


Rasteriser::Rasteriser(SDL_Surface *screen,Model * model,Camera &camera,Lighting &lighting) : Renderer(screen), model(model), camera(camera ), lighting(lighting) {
  this->depth = 1000;
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
  if (std::abs(u[2])>1e-2){
		//u.z is the area of all 3 vertices
		//Divide this by the are of two vertices and the test point to get baycentric value
		return vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
	}

  return vec3(-1,1,1);
}

void Rasteriser::DrawPolygon(vec4 * verticies, int polyEdgeCount, Shader &shader, float *z_buffer, bool draw_screen) {


	int triangleCount = 1;

	//todo generalise this
	if(polyEdgeCount == 4){
		triangleCount = 2;
	}else if(polyEdgeCount == 5){
		triangleCount = 3;
	}

	vec4 v0 ;
	vec4 v1 ;
	vec4 v2 ;

	for(int i = 0 ; i < triangleCount ; i ++) {

		//todo generalise this too
		if(i == 0){

			v0 = verticies[2];
			v1 = verticies[0];
			v2 = verticies[1];


		}else if (i ==1 ){
			v0 = verticies[2];
			v1 = verticies[3];
			v2 = verticies[0];

		}else if (i ==2 ){
			v0 = verticies[0];
			v1 = verticies[3];
			v2 = verticies[4];

		}



		vec2 bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		vec2 bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				bboxmin[j] = std::min(bboxmin[j], verticies[i][j] / verticies[i][3]);
				bboxmax[j] = std::max(bboxmax[j], verticies[i][j] / verticies[i][3]);
			}
		}

		//todo integrate this with the code above


		for (int x = bboxmin.x; x <= bboxmax.x; x++) {
			for (int y = bboxmin.y; y <= bboxmax.y; y++) {

				vec3 c = barycentric(vec2(v0.x / v0.w, v0.y / v0.w),
														 vec2(v1.x / v1.w, v1.y / v1.w),
														 vec2(v2.x / v2.w, v2.y / v2.w),
														 vec2(x, y));
				float z = v0.z * c.x + v1.z * c.y + v2.z * c.z;
				float w = v0.w * c.x + v1.w * c.y + v2.w * c.z;
				int frag_depth = z / w;
				if (c.x < 0 || c.y < 0 || c.z < 0 || z_buffer[x + y * width] > frag_depth) continue;
				vec3 colour;
				shader.fragment(c, colour);
				z_buffer[x + y * width] = frag_depth;
				if (draw_screen)PutPixelSDL(screen, x, height - (y + 1), colour);

			}
		}
	}
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


const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000



inline float cross (vec2 a , vec2 b){
	return a.x * b.y - a.y * b.x;
}


inline bool rhs(vec2 a, vec2 b, vec2 p){
	vec2 t1, t2;
	t1 = b -a;
	t2 = p -b;
	float x = cross(t1,t2);
	return x < 0;
}

inline vec4 intersection(vec4 a, vec4 b, vec4 c , vec4 d){
	//todo might be a faster way of doing this?
	float l = cross(c-a, d-c) / cross(b-a,d-c);
	return a + l*(b-a);
}

void Rasteriser::Clip(vec4 vertex[3], vec4 ** clipped, int * count) {


	vec2 clip_edge [4] = { vec2(0,0), vec2(0,height) ,
		vec2(width, height), vec2 (width,0)
	};

	//todo remove mallocs from function call
	vec4 * inList = (vec4*)malloc(sizeof(vec4) * 10);
	int inListCount = 0;

	for(int i = 0 ; i < 3 ; i++){
		inList[inListCount++] = vertex[i];
	}

	vec4 * outList = (vec4*)malloc(sizeof(vec4) * 10);
	vec4 * temp;

	int outListCount = 0;

	for(int clip = 0; clip < 4 ; clip++){

		int next_clip = (clip + 1) % 4;
		vec2 vecClip = clip_edge[next_clip] - clip_edge[clip];
		outListCount = 0;

		for (int inVertexPtr = 0; inVertexPtr < inListCount ; inVertexPtr++)
		{
			int nextVertexPtr = (inVertexPtr + 1) % inListCount;

			bool in_current = rhs(clip_edge[clip],clip_edge[next_clip],inList[inVertexPtr]);
			bool in_next = rhs(clip_edge[clip],clip_edge[next_clip],inList[nextVertexPtr]);

			if(in_current && in_next){
				outList[outListCount++] = inList[nextVertexPtr];
				continue;
			}else if (!in_current && !in_next){
				continue;
			}

			vec4 intersect = intersection(inList[inVertexPtr],inList[nextVertexPtr]
							,vec4(clip_edge[clip],0,0),vec4(clip_edge[next_clip],0,0));

			if(in_current && !in_next){
				outList[outListCount++] = intersect;
			}else if(!in_current && in_next){
				outList[outListCount++] = intersect;
				outList[outListCount++] = inList[nextVertexPtr] ;
			}
		}

		temp = inList;
		inList = outList;
		outList = temp;
		inListCount = outListCount;
	}


	*count = inListCount;
	*clipped = inList;
}

void Rasteriser::Draw()
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

  vec4 vetex[3];

	//todo split into tiles

  int renderCount = model->triangleCount();



	vec4 * outlist = (vec4*)malloc(sizeof(vec4)*20);
  for(int i = 0 ; i < renderCount; i++){
    for(int j = 0; j < 3 ;j++){
      vetex[j] = depthShader.proj(i,j);
    }

		int count;
		Clip(vetex,&outlist,&count);
		DrawPolygon(outlist, count, depthShader, depthBufferLight, false);
	}

  LookAt(camera_pos, center, up);
  Projection(-1.f/ length(camera_pos-center));

  mat4 camera_light =  inverse(modelView * projection * viewPort) * MM;

  Shadow shadowShader(this, camera_light, modelView);

  for(int i = 0 ; i <renderCount; i++){
    for(int j = 0; j < 3 ;j++){
      vetex[j] = shadowShader.proj(i,j);
    }
		outlist = (vec4*)malloc(sizeof(vec4)*20);
		int count;
		Clip(vetex,&outlist,&count);
		DrawPolygon(outlist, count, shadowShader, depthBufferCamera, true);

  }


  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }

  SDL_UpdateRect( screen, 0, 0, 0, 0 );
  return;
}



