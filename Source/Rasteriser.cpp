#include <iostream>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Lighting.h"
#include "Rasteriser.h"
//#include "Renderer.h"
#define MAX_VERTICIES 10

//Coordinates of the current polygon to draw

bool Rasteriser::DepthShader::colour(glm::vec3 bar, glm::vec3 &colour, Polygon *triangle, RenderData *data) {

  vec4 point = data->renderSpaceVertices * bar;
  vec3 p = point / point.w * viewPort;


  colour = vec3(255, 255, 255) * p / this->r->depth;
  return true;
}


bool Rasteriser::Shadow::colour(glm::vec3 bar, glm::vec3 &colour, Polygon *triangle, RenderData *data) {

  mat3x4 m = data->renderSpaceVertices;
	mat3x4 ma = data->worldSpaceVerticies;
	vec3 world_point = ma*bar;

	vec4 aa = ma*bar * screen_shadow ;
	aa = aa/aa.w * viewPort;

  vec4 light =  aa ;

  float shadow = 1;

  int xx = int(light[0]); //Indexes into light buffer
  int yy = int(light[1]);

  if(xx < r->width && yy < r->height && xx >= 0 && yy >= 0){
    int idx =  int(light[0]) + int(light[1])*r->width;
    shadow = (r->depthBufferLight[idx] < (light[2] + 200 ));
  }

  //If we have textures, handle differently compared to if we just have materials
  if(r->model->loadedNormalTexture){


		mat3x2 text;
		text[0]  = data->textureCoordinates[0];
		text[1]  = data->textureCoordinates[1];
		text[2]  = data->textureCoordinates[2];

		vec2 textureCoordInterp =  text * bar;

    vec4 normal = vec4(r->model->normalMapTexture(textureCoordInterp),1); // normal

    mat4 mm = transpose(inverse(projection));

    vec3 vv = vec3(normal*mm);
    vec3 n =  normalize( vv);

    vec3 ll = (vec3)(vec4(r->light_pos,1) );
    vec3 l = normalize(ll);


    float ttt = glm::dot(n,l)*2.0f;
    vec3 tt = n*ttt - l;
    vec3 ref = normalize(tt);

    float spec = pow(std::max<float>(ref.z, 0.0f), r->model->specularTexture(textureCoordInterp));

    float diff = std::max<float>(0.f, glm::dot(n,l));

    unsigned char * diffuse = r->model->diffuseTexture(textureCoordInterp);

      //for (int i=0; i<3; i++) colour[i] =  c[i] ;
    for (int i=0; i<3; i++) colour[2-i] = std::min<float>(20.0f + diffuse[i]*shadow*(1.2f* spec+ 0.2*diff), 255);

    //colour = intensity * std::min<float>(shadow, 1) * cc ;

  }else{

    if(triangle->material>=0){


      //Calculate normal of the three vertices
      vec3 e1 = (vec3)(ma[2]-ma[1]);
      vec3 e2 = (vec3)(ma[1]-ma[0]);
      vec3 verticesNormal = glm::normalize( glm::cross( e2, e1 ) );

			float aIntensity = 0.6;
      vec3 ambient(aIntensity,aIntensity,aIntensity);

      // Materia file values
      vec3 ka = r->model->ambiantReflectance(triangle->material);
      vec3 kd = r->model->diffuseReflectance(triangle->material);
      vec3 ks = r->model->specularReflectance(triangle->material);
      vec3 ke = r->model->glowReflectance(triangle->material);


			//Phong light implementation

			vec3 L = normalize(r->light_pos - world_point);
			vec3 N = verticesNormal;
			vec3 V =  normalize(r->camera_pos - world_point);
			vec3 C = N * (glm::dot(L, N));
			vec3 R = 2.0f*C - L;
      vec3 l = normalize(r->light_pos);
      vec3 H = normalize((V + l));

      //We use a scaling factor so that light drops off
			float lengthLightDistance = length( world_point - r->light_pos) * 3.0f ;



      float lightVerticesNormal = std::max<float>(0,glm::dot(verticesNormal , l ));

			float NdotH = glm::dot( N, H );

      for(int i = 0 ; i < 3;i++){
        colour[i] = 255.0f * std::min<float>(
								ke[i] +
								1 * shadow * ka[i] * ambient[i] * (r->lighting.colour()[i] / (4 * 3.14f * lengthLightDistance * lengthLightDistance)) +
								0.4 * kd[i] *  lightVerticesNormal ,1) ;
      }

      //todo implement the rest of the shading for the other mmaterialtypes;
      /* Ka * Ia + Kd * (N * L0) * Ij
       *
       * 1 This is a diffuse illumination model using Lambertian shading. The color includes an ambient and diffuse shading terms for each light source. The formula is
      color = KaIa + Kd { SUM j=1..ls, (N * Lj)Ij }


       P:
       L: unit vector towards light source from P
			 N: Normal Vector N


      2 This is a diffuse and specular illumination model using Lambertian shading and Blinn's interpretation of Phong's specular illumination model (BLIN77).
      The color includes an ambient constant term, and a diffuse and specular shading term for each light source. The formula is:
      color = KaIa + Kd { SUM j=1..ls, (N*Lj)Ij } + Ks { SUM j=1..ls, ((H*Hj)^Ns)Ij }

      Term definitions are: Ia ambient light, Ij light j's intensity,
       Ka ambient reflectance,
       Kd diffuse reflectance,
       Ks specular reflectance, H unit vector bisector between L and V,
       L unit light vector, N unit surface normal, V unit view vector
      *


      */



    }else{
      colour = vec3(0,0,0);
    }
  }



  return true;
}


Rasteriser::Rasteriser(SDL_Surface *screen,Model * model,Camera &camera,Lighting &lighting) : Renderer(screen), model(model), camera(camera ), lighting(lighting) {
  this->depth = 10000;
	this->screen = screen;
	this->width = screen->w;//todo remove this
	this->height = screen->h;//todo remove this
	this->dimensions[0] = this->width;
	this->dimensions[1] = this->height;
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
  //Else return negative values, so our check fails
  return vec3(-1,-1,-1);
}

void Rasteriser::DrawTriangle(RenderData *data, Shader &shader, float *z_buffer, Polygon *triangle, bool draw_screen) {


  ivec2 min(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
  ivec2 max(-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());

  vec4 vertices[3];
  for (int i = 0; i < 3; i++) {

    vertices[i] = data->renderSpaceVertices[i]/data->renderSpaceVertices[i].w * viewPort;
    for (int j = 0; j < 2; j++) {
      min[j] = std::min(min[j], (int)vertices[i][j] );
      max[j] = std::max(max[j], (int)vertices[i][j] );
    }
  }


#if DEBUG
		if(min[0] < 0 || max[0] > width || min[1] < 0 || max[1] > height ){
			cout << "Range error " << min[0] "," min[1] << "|" << max[0] "," max[1] << endl;
		}
#endif

  //After clipping we should be safe to set a hard limit on the bounds.
  //Compile with debug mode for a warning, for out of bound items
	for(int j = 0 ; j < 2 ; j ++) {
		min[j] = std::max(min[j], 0);
		max[j] = std::min(max[j], dimensions[j]-1);
	}

  for (int y = min.y; y <= max.y; y++) {
    for (int x = min.x; x <= max.x; x++) {

      vec3 bar = barycentric(vec2(vertices[0].x , vertices[0].y ),
                             vec2(vertices[1].x , vertices[1].y),
                             vec2(vertices[2].x , vertices[2].y ),
                             vec2(x, y));

      float z =  bar.x/vertices[0].z +  bar.y/vertices[1].z  +  bar.z/ vertices[2].z;
      z = 1/z;

      if (bar.x < 0 || bar.y < 0 || bar.z < 0 || z_buffer[x + y * width] > z) continue;
      vec3 colour;
      shader.colour(bar, colour, triangle, data);
      z_buffer[x + y * width] = z;
			if (draw_screen)PutPixelSDL(screen, x, height - (y + 1), colour);

    }
  }

}



void Rasteriser::DrawPolygon(vec4 *vertices, vec2 *inTextures, int polyEdgeCount, Shader &shader, float *z_buffer, Polygon *triangle,
                        bool draw_screen) {

	if(polyEdgeCount == 0){
		return;
	}

  int triangleCount = polyEdgeCount - 2;
  RenderData data;

	for(int i = 0; i < triangleCount; i ++) {
//Breaks the polygon up into triangles
    data.renderSpaceVertices[0] = vertices[0];
    data.renderSpaceVertices[1] = vertices[1+i];
    data.renderSpaceVertices[2] = vertices[2+i];

		data.worldSpaceVerticies[0] = vertices[0] * inverse(modelView*projection);
		data.worldSpaceVerticies[1] = vertices[1+i] * inverse(modelView*projection);
		data.worldSpaceVerticies[2] = vertices[2+i] * inverse(modelView*projection);

		data.worldSpaceVerticies[0] = data.worldSpaceVerticies[0]/data.worldSpaceVerticies[0].w;
		data.worldSpaceVerticies[1] = data.worldSpaceVerticies[1]/data.worldSpaceVerticies[1].w;
		data.worldSpaceVerticies[2] = data.worldSpaceVerticies[2]/data.worldSpaceVerticies[2].w;

    if(inTextures != NULL) {
      data.textureCoordinates[0] = inTextures[0];
      data.textureCoordinates[1] = inTextures[1 + i];
      data.textureCoordinates[2] = inTextures[2 + i];
    }

    DrawTriangle(&data, shader, z_buffer, triangle, draw_screen);
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


#define W_CLIP 0.00001

//todo find an efficient way of rendering without textures
void Clip(vec4 *inVertices, vec2 * inTextures , int inCount , vec4 * retVertices, vec2 * retTextures , int * retCount) {

	vec4 outVerticesArray[MAX_VERTICIES];
	vec2 outTexturesArray[MAX_VERTICIES];


	vec4 * outVertices = outVerticesArray;
	vec4 * tempVertices;


	vec2 * outTextures = outTexturesArray;
	vec2 * tempTexture;


	int currentIn, previousIn;
	int outCount = 0;

	int previousVertex = inCount - 1;
	previousIn = inVertices[previousVertex].w < W_CLIP ? 0 : 1;
	for(int currentVertex = 0; currentVertex < inCount ; currentVertex++){

		currentIn = inVertices[currentVertex].w < W_CLIP ? 0 : 1;

		if ( currentIn != previousIn  ) {

			float ifactor;
			ifactor = (float) ((W_CLIP - inVertices[previousVertex].w) /
												 (inVertices[previousVertex].w - inVertices[currentVertex].w));
			vec4 ip = inVertices[previousVertex] - ifactor * (inVertices[currentVertex] - inVertices[previousVertex]);
      outVertices[outCount] = ip;

      if(retTextures != NULL) {

        vec2 ipTexture =
                inTextures[previousVertex] + ifactor * (inTextures[currentVertex] - inTextures[previousVertex]);
        outTextures[outCount] = ipTexture;
      }
      outCount++;


		}

		//If the current doesn't need to be clipped, add it to the list
		if (currentIn){
			outVertices[outCount] = inVertices[currentVertex];
			if(retTextures != NULL) outTextures[outCount] = inTextures[currentVertex];
      outCount++;
		}
		previousVertex = currentVertex;
		previousIn = currentIn;
	}

	//Clip other axis

	inVertices = outVertices;
	inTextures = outTextures;

	inCount = outCount;
	outCount = 0;




	vec4 tempArray[MAX_VERTICIES];
	outVertices = tempArray;

	//Clip on each axis
	for (int axis = 0; axis < 3 ; axis ++) {

		//+ and - ve clip
		for (int i = 0; i < 2; i++) {

			//Exit if we have clipped all vertices
			if(inCount < 1){
				*retCount = 0;
				return;
			}

			int j = (i % 2 == 1 ? -1 : 1);

			int previousVertex = inCount - 1;

			previousIn = j * inVertices[previousVertex][axis] > inVertices[previousVertex].w ? 0 : 1;

			for (int currentVertex = 0; currentVertex < inCount; currentVertex++) {

				currentIn = j * inVertices[currentVertex][axis] > inVertices[currentVertex].w ? 0 : 1;

				if (previousIn != currentIn) {


					float ifactor = (inVertices[previousVertex].w - j * inVertices[previousVertex][axis]) /
													(
																	(inVertices[previousVertex].w - j * inVertices[previousVertex][axis]) -
																	(inVertices[currentVertex].w - j * inVertices[currentVertex][axis])
													);


					vec4 ip = inVertices[previousVertex] +
										ifactor * (inVertices[currentVertex] - inVertices[previousVertex]);

          if(retTextures != NULL) {

            vec2 ipTexture = inTextures[previousVertex] +
                             ifactor * (inTextures[currentVertex] - inTextures[previousVertex]);
            outTextures[outCount] = ipTexture;
          }
					outVertices[outCount] = ip;
          outCount++;
				}


				//If the currents doesn't need to be clipped, add it to the list
				if (currentIn) {
					outVertices[outCount] = inVertices[currentVertex];

          if(retTextures != NULL)outTextures[outCount]=inTextures[currentVertex];

          outCount++;
				}
				previousVertex = currentVertex;
				previousIn = currentIn;
			}

			tempVertices = outVertices;
			outVertices = inVertices;
			inVertices = tempVertices;

      if(retTextures != NULL) {

        tempTexture = outTextures;
        outTextures = inTextures;
        inTextures = tempTexture;

      }


			inCount = outCount;
			outCount = 0;

		}
	}
	memcpy(retVertices,inVertices,inCount*sizeof(vec4));
	if(retTextures!=NULL)memcpy(retTextures, inTextures,inCount*sizeof(vec2));
	*retCount = inCount;

}

void Rasteriser::ProcessPolygons(Model *model, Renderer::Shader &shader, float *z_buffer, vec4 *vertices,
                                 vec2 *textures, vec4 *outVertices, vec2 *outTextures, bool clip ,bool draw_screen)
{
  int renderCount = model->triangleCount();
  int count;
  for(int i = 0; i < renderCount; i++) {

    Polygon * triangle = model->GetTriangle(i);

    for(int j = 0; j < triangle->verticesCount ;j++) {

			vec4 v = vec4(model->vertex(i,j),1);
      vertices[j] = v * modelView * projection;

      if(outTextures != NULL) {
        if(model->loadedNormalTexture) {
          vec2 t = model->textureCoordinate(i, j);
          textures[j] = t;
        }else{
          outTextures = NULL;
        }
      }
    }
		if(clip) {
      Clip(vertices, textures, triangle->verticesCount, outVertices, outTextures, &count);
      DrawPolygon(outVertices, outTextures, count, shader, z_buffer, triangle, draw_screen);
    }else{
      DrawPolygon(vertices, textures, triangle->verticesCount, shader, z_buffer, triangle, draw_screen);
    }
  }
}



void Rasteriser::Draw(bool clip)
{

  SDL_FillRect( screen, 0, 0 );

  if( SDL_MUSTLOCK(screen) ) {
    SDL_LockSurface(screen);
  }

	for(int i = 0 ; i < height * width ; i ++){
		depthBufferCamera[i] = -INFINITY;
		depthBufferLight[i] = -INFINITY;
	}

	ViewPort(0, 0, width-1, height-1);
  vec3 centerO(0,0,0);
  vec3 up(0,1,0);
	vec3 center;

	float l;

  light_pos = lighting.position();
  camera_pos = camera.position();
	vec3 dir = camera.direction();

  Projection(-1.0f);

  vec3 d(0,0,-1);

  LookAt(light_pos , d+light_pos , vec3(0,1,0) );
  //
  //Store the transformation for use in the shader


  DepthShader depthShader(this);

  vec4 vertices[MAX_VERTICIES];
	vec2 textures[MAX_VERTICIES];


	int count;
	vec4 * outVertices = (vec4*)malloc(sizeof(vec4) * MAX_VERTICIES);
  vec2 * outTextures = (vec2*)malloc(sizeof(vec4) * MAX_VERTICIES);


  mat4 MM = modelView * projection ;



  ProcessPolygons(model, depthShader, depthBufferLight, vertices, NULL, outVertices, NULL, clip,false);


  Projection(-1.f/ length(camera_pos-(camera_pos+dir)));



	//LookAt(camera_pos, camera_pos+dir, vec3(0,1,0));

  LookAt(camera_pos, camera_pos+dir, vec3(0,1,0));

  //Transform from camera space to light space
  mat4 camera_transform =  MM;

  Shadow shadowShader(this, camera_transform);
  ProcessPolygons(model, shadowShader, depthBufferCamera, vertices, textures, outVertices, outTextures, clip, true);

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }

  SDL_UpdateRect( screen, 0, 0, 0, 0 );
  return;
}



