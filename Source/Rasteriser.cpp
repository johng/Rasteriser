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
  vec4 retval = vertex * modelView * projection ;
  //vec4 retval = vertex * modelView * projection * viewPort;
  for(int i = 0; i < 3 ; i ++){
    //tri[i][index] = retval[i]/retval.w;
  }
  return retval;
}

bool Rasteriser::DepthShader::fragment(vec3 bar, vec3 & colour) {
  vec3 p = bar*tri;
  //colour = vec3(1, 1, 1)*(p.z/depth);
  colour = vec3(255, 255, 255);
  return true;
}


vec4 Rasteriser::Shadow::proj(int triangle_index, int index) {
  textureCoordinates[index] = r->model->textureCoordinate(triangle_index,index);
  vec4 vertex = vec4(r->model->vertex(triangle_index,index),1);
  //vec4 retval = vertex * modelView * projection * viewPort;
  vec4 retval = vertex * modelView * projection ;
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
  for (int i=1; i>=0; i--) {
    s[i][0] = C[i]-A[i];
    s[i][1] = B[i]-A[i];
    s[i][2] = A[i]-P[i];
  }
  vec3 u = cross(s[0], s[1]);
  if (std::abs(u[2])>0.0001){
		//u.z is the area of all 3 vertices
		//Divide this by the are of two vertices and the test point to get baycentric value
		return vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
	}
  return vec3(-1,1,1);
}
void Rasteriser::DrawPolygon(vec4 * verticies, int polyEdgeCount, Shader &shader, float *z_buffer, bool draw_screen) {


	int triangleCount = 1;

	if(polyEdgeCount == 0){
		return;
	}

	//todo generalise this
	if(polyEdgeCount == 4){
		triangleCount = 2;
	}else if(polyEdgeCount == 5){
		triangleCount = 3;
	}

	vec4 drawVerticies[3];

	for(int i = 0 ; i < triangleCount ; i ++) {

		//todo generalise this too
		if(i == 0){

			drawVerticies[0] = verticies[0];
			drawVerticies[1] = verticies[1];
			drawVerticies[2] = verticies[2];


		}else if (i ==1 ){
			drawVerticies[0] = verticies[0];
			drawVerticies[1] = verticies[2];
			drawVerticies[2] = verticies[3];

		}else if (i == 2 ){
			drawVerticies[0] = verticies[0];
			drawVerticies[1] = verticies[3];
			drawVerticies[2] = verticies[4];
		}


		ivec2 bboxmin(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
		ivec2 bboxmax(-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());


		for (int i = 0; i < 3; i++) {

			drawVerticies[i] = (drawVerticies[i]/drawVerticies[i].w) * viewPort;
			for (int j = 0; j < 2; j++) {
				bboxmin[j] = std::min(bboxmin[j], (int)drawVerticies[i][j] );
				bboxmax[j] = std::max(bboxmax[j], (int)drawVerticies[i][j] );
			}
		}

		for (int x = bboxmin.x; x <= bboxmax.x; x++) {
			for (int y = bboxmin.y; y <= bboxmax.y; y++) {

				if(x < 0 || y < 0 || x > width || y > height){
					cout << x << "," << y << "\n";
					continue;
				}

				vec3 bar = barycentric(vec2(drawVerticies[0].x , drawVerticies[0].y ),
														 vec2(drawVerticies[1].x , drawVerticies[1].y),
														 vec2(drawVerticies[2].x , drawVerticies[2].y ),
														 vec2(x, y));
				float z = drawVerticies[0].z * bar.x + drawVerticies[1].z * bar.y + drawVerticies[2].z * bar.z;
				if (bar.x < 0 || bar.y < 0 || bar.z < 0 || z_buffer[x + y * width] > z) continue;
				vec3 colour;
				shader.fragment(bar, colour);
				z_buffer[x + y * width] = z;
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


inline float cross (vec2 a , vec2 b){
	return a.x * b.y - a.y * b.x;
}

inline bool rhs(vec2 a, vec2 b, vec2 p){
	vec2 t1, t2;
	t1 = b -a;
	t2 = p -b;
	float x = cross(t1,t2);
	return x <= 0;
}

#define W_CLIP 0.0000001

void ClipWAxis(vec4 *verticies, vec4 *outVerticies, int count, int *outCount) {

	vec4 tempVerticies [10];

	vec4 currentV, previousV;

	int currentIn, previousIn;
	int tempCount = 0;

	int previousVertex = count - 1;
	previousIn = verticies[previousVertex].w < W_CLIP ? 0 : 1;
	for(int currentVertex = 0; currentVertex < count ; currentVertex++){

		currentIn = verticies[currentVertex].w < W_CLIP ? 0 : 1;

		if ( currentIn != previousIn  ) {

			float ifactor;
			ifactor = (float) ((W_CLIP - verticies[previousVertex].w) /
												 (verticies[previousVertex].w - verticies[currentVertex].w));
			vec4 ip = verticies[previousVertex] + ifactor * (verticies[currentVertex] - verticies[previousVertex]);
			tempVerticies[tempCount++] = ip;
		}

		//If the current doesn't need to be clipped, add it to the list
		if (currentIn){
			tempVerticies[tempCount++] = verticies[currentVertex];
		}
		previousVertex = currentVertex;
		previousIn = currentIn;
	}
	memcpy(outVerticies, tempVerticies, tempCount * sizeof(vec4));
	*outCount = tempCount;
}

void ClipAxis(vec4 * verticies, vec4 * oVerticies , int inCount, int * passOnCount ){


	vec4 * outVerticies = (vec4*)malloc(sizeof(vec4) * 10);

	vec4 * inVerticies = verticies;
	vec4 * temp;

	int currentIn, previousIn;
	int outCount = 0;

	//Clip on each axis
	for (int axis = 0; axis < 3 ; axis ++) {

		//+ and - ve clip
		for (int i = 0; i < 2; i++) {

			//Exit if we have clipped all vertices
			if(inCount == 0){
				* passOnCount = 0;
				return;
			}

			int j = (i % 2 == 1 ? -1 : 1);

			int previousVertex = inCount - 1;

			previousIn = j * inVerticies[previousVertex][axis] > inVerticies[previousVertex].w ? 0 : 1;

			for (int currentVertex = 0; currentVertex < inCount; currentVertex++) {

				currentIn = j * inVerticies[currentVertex][axis] > inVerticies[currentVertex].w ? 0 : 1;

				if (previousIn != currentIn) {


					float ifactor = (inVerticies[previousVertex].w - j * inVerticies[previousVertex][axis]) /
													(
																	(inVerticies[previousVertex].w - j * inVerticies[previousVertex][axis]) -
																	(inVerticies[currentVertex].w - j * inVerticies[currentVertex][axis])
													);


					vec4 ip = inVerticies[previousVertex] +
										ifactor * (inVerticies[currentVertex] - inVerticies[previousVertex]);

					outVerticies[outCount++] = ip;

				}


				//If the currents doesn't need to be clipped, add it to the list
				if (currentIn) {
					outVerticies[outCount++] = inVerticies[currentVertex];
				}
				previousVertex = currentVertex;
				previousIn = currentIn;
			}

			temp = outVerticies;
			outVerticies = inVerticies;
			inVerticies = temp;
			inCount = outCount;
			outCount = 0;

		}
	}

	memcpy(oVerticies,inVerticies , sizeof(vec4)*inCount);
	*passOnCount = inCount;

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
  ViewPort(0, 0, width, height);


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
		ClipWAxis(vetex, outlist, 3, &count);
		//DrawPolygon(outlist, count, depthShader, depthBufferLight, false);
	}

  LookAt(camera_pos, center, up);
  Projection(-1.f/ length(camera_pos-center));

  mat4 camera_light =  inverse(modelView * projection * viewPort) * MM;

  Shadow shadowShader(this, camera_light, modelView);

  for(int i = 0 ; i <renderCount; i++){
    for(int j = 0; j < 3 ;j++){
      vetex[j] = depthShader.proj(i,j);
    }
		outlist = (vec4*)malloc(sizeof(vec4)*20);
		int count;

		ClipWAxis(vetex, outlist, 3, &count);
		ClipAxis(outlist,outlist,count,&count);
		DrawPolygon(outlist, count, depthShader, depthBufferCamera, true);

  }

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }

  SDL_UpdateRect( screen, 0, 0, 0, 0 );
  return;
}



