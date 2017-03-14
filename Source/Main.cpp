#include "Rasteriser.h"
#include "TestModel.h"
#include "Lighting.h"

bool ProcessInput(int& t, Camera & camera);
using namespace std;
using glm::vec3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

int main(int argc, char* argv[] )
{
	vector<Triangle> triangles;

	vec3 camPos (-0.5,-0.5,-3);
	mat4 lightingPos(1,0,0,0.1
									,0,1,0,0.2
									,0,0,1,3
									,0,0,0,1);

  vec3 lightColour(10,10,10);
	Camera camera(camPos);
	Lighting lighting(lightingPos, lightColour);

	SDL_Surface *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	int t = SDL_GetTicks();	// Set start value for timer.

	LoadTestModel( triangles );

	//SDL_WM_GrabInput( SDL_GRAB_ON );
	//SDL_ShowCursor(0);

	Rasteriser r (screen) ;

	//A bit of a hack to flush initial events
	SDL_Event e;
	//while( SDL_PollEvent(&e) );
	int i = 0;
	//while(i++ < 2 )
	while( NoQuitMessageSDL() )
	{
		r.Draw(camera,lighting,triangles);
		ProcessInput(t,camera);
	}
	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}


bool ProcessInput(int& t, Camera & camera )
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	float sf = 0.002f;
	cout << "Render time: " << dt << " ms." << endl;
	cout << "Camera pos: " << camera << endl;
	Uint8* keystate = SDL_GetKeyState( 0 );


	vec3 forward (0,0,1);
	vec3 down (0,1,0);
	vec3 right (1,0,0);

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
