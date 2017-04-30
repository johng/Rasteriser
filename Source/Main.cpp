#include "Rasteriser.h"
#include "Lighting.h"
#include "Model.h"
#include "Texture.h"
bool ProcessInput(int& t, Camera & camera , Lighting & lighting , bool debug );
using namespace std;
using glm::vec3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;


int main(int argc, char* argv[] )
{


	Model model;

	//todo cli loader
  model.LoadMaterialsFile("./Models/cornell-box/CornellBox-Original.mtl");
  model.LoadObj("./Models/cornell-box/CornellBox-Original.obj");


	//model.LoadObj("./Models/Diablo/diablo3_pose.obj");
  //model.LoadDiffuseTexture("./Models/Diablo/diablo3_pose_diffuse.tga");
  //model.LoadNormalMap("./Models/Diablo/diablo3_pose_nm.tga");
	//model.LoadSpecularTexture("./Models/Diablo/diablo3_pose_spec.tga");


	bool debug = false;
	bool show_screen = true;
	for(int i = 1 ; i < argc; i++){
		char * arg = argv[i] ;
		if(strcmp(arg,"-debug") == 0){
			debug = true;
			cout << "Debugging enabled\n";
		}

		if(strcmp(arg,"-dev") == 0){
			show_screen = false;
			cout << "Dev mode\n";
		}

	}

	Camera camera(vec3(0,1,2), vec3(0,0,-1));
	Lighting lighting(vec3(0,1,4), vec3(1,1,1)*400.0f);

	SDL_Surface *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	int t = SDL_GetTicks();	// Set start value for timer.



	Rasteriser r (screen, &model,camera,lighting) ;


	if(show_screen) {
		while (NoQuitMessageSDL()) {
			ProcessInput(t, camera, lighting , debug);
			r.Draw();
		}
	}else{
		r.Draw();
		int t2 = SDL_GetTicks();
		float dt = float(t2-t);
		t = t2;
		cout << "Render time: " << dt << " ms." << endl;
		cout << "Camera: " <<  camera << endl;

	}
	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}


bool ProcessInput(int& t, Camera & camera , Lighting & lighting , bool debug )
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	//float sf = 0.002f;

	if(debug) {
		cout << "Render time: " << dt << " ms." << endl;
		cout << "Camera pos: " << camera << endl;
		cout << "Lighting pos: " << lighting << endl;
	}
	Uint8* keystate = SDL_GetKeyState( 0 );

	float delta_move = 0.02f;

	vec3 forward (0,0,delta_move) ;
	vec3 up (0,delta_move,0);
	vec3 right (delta_move,0,0);


	//Camera Position
	if( keystate[SDLK_w] )
	{
		// Move camera forward
		camera.moveDir(up);
	}
	if( keystate[SDLK_s] )
	{
		// Move camera backward
    camera.moveDir(-up);
	}
	if( keystate[SDLK_a] )
	{
		// Move camera to the left
		camera.moveDir(-right);

	}
	if( keystate[SDLK_d] )
	{
		// Move camera to the right
		camera.moveDir(right);
	}

	if( keystate[SDLK_UP] )
	{
		// Move camera forward
    camera.movePos(up);
	}
	if( keystate[SDLK_DOWN] )
	{
		// Move camera backward
		camera.movePos(-up);
	}
	if( keystate[SDLK_RIGHT] )
	{
		// Move camera to the left
		camera.movePos(right);
	}
	if( keystate[SDLK_LEFT] )
	{
		// Move camera to the right
		camera.movePos(-right);
	}


	if( keystate[SDLK_e] )
	{
		// Move camera to the right
		camera.movePos(forward);
	}

	if( keystate[SDLK_q] )
	{
		// Move camera to the right
		camera.movePos(-forward);
	}


  if( keystate[SDLK_h] )
  {
    // Move camera to the right
    lighting.move(-right);
  }

  if( keystate[SDLK_k] )
  {
    // Move camera to the right
    lighting.move(right);
  }

  if( keystate[SDLK_u] )
  {
    // Move camera to the right
    lighting.move(up);
  }

  if( keystate[SDLK_j] )
  {
    // Move camera to the right
    lighting.move(-up);
  }





  if( keystate[SDLK_ESCAPE] )
	{
		//Quit
		return false;
	}

	//SDL_Event e;

	return true;
}
