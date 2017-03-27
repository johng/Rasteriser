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

  //Texture tgaImage;
  //tgaImage.ReadTGAImage("./Models/Diablo/diablo3_pose_diffuse.tga");

	Model model;
	//model.LoadObj("./Models/cornell-box/CornellBox-Glossy.obj");
	model.LoadObj("./Models/Diablo/diablo3_pose.obj");
  model.LoadDiffuseTexture("./Models/Diablo/diablo3_pose_diffuse.tga");
  model.LoadNormalMap("./Models/Diablo/diablo3_pose_nm.tga");
	model.LoadSpecularTexture("./Models/Diablo/diablo3_pose_spec.tga");

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

	Camera camera(vec3(1,1,4));
	Lighting lighting(vec3(1,1,0), vec3(1,1,1)*14.0f);

	SDL_Surface *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	int t = SDL_GetTicks();	// Set start value for timer.


	//SDL_WM_GrabInput( SDL_GRAB_ON );
	//SDL_ShowCursor(0);

	Rasteriser r (screen, &model) ;

	//A bit of a hack to flush initial events
	SDL_Event e;
	//while( SDL_PollEvent(&e) );

	if(show_screen) {
		while (NoQuitMessageSDL()) {
			ProcessInput(t, camera, lighting , debug);
			r.Draw(camera, lighting);
		}
	}else{

		r.Draw(camera, lighting);
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
	float sf = 0.002f;

	if(debug) {
		cout << "Render time: " << dt << " ms." << endl;
		cout << "Camera pos: " << camera << endl;
		cout << "Lighting pos: " << lighting << endl;
	}
	Uint8* keystate = SDL_GetKeyState( 0 );

	float delta_move = 0.25f;

	vec3 forward (0,0,1);
	vec3 down (0,1,0);
	vec3 right (1,0,0);

	forward *= delta_move;
	down *= delta_move;
	right *= delta_move;

	//forward *= sf * dt;
	//right *= sf * dt;

	//Camera Position
	if( keystate[SDLK_w] )
	{
		// Move camera forward
		camera.move(-down);
	}
	if( keystate[SDLK_s] )
	{
		// Move camera backward
		camera.move(down);
	}
	if( keystate[SDLK_a] )
	{
		// Move camera to the left
		camera.move(right);
	}
	if( keystate[SDLK_d] )
	{
		// Move camera to the right
		camera.move(-right);
	}



	if( keystate[SDLK_UP] )
	{
		// Move camera forward
		lighting.move(-down);
	}
	if( keystate[SDLK_DOWN] )
	{
		// Move camera backward
		lighting.move(down);
	}
	if( keystate[SDLK_RIGHT] )
	{
		// Move camera to the left
		lighting.move(right);
	}
	if( keystate[SDLK_LEFT] )
	{
		// Move camera to the right
		lighting.move(-right);
	}

	if( keystate[SDLK_ESCAPE] )
	{
		//Quit
		return false;
	}

	//SDL_Event e;

	//Single hit keys and mouse movement
	/*
	while( SDL_PollEvent(&e) )
	{
		if( e.type == SDL_QUIT )
			return false;
		if( e.type == SDL_KEYDOWN )
		{
			if( e.key.keysym.sym == SDLK_ESCAPE)
				return false;
		}
		if( e.type == SDL_MOUSEMOTION )
		{
			camera.rotate(-sf * e.motion.yrel, sf * e.motion.xrel);
		}
	}
	 */
	return true;
}
