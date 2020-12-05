#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"

#include "entity.h"
#include "entitymesh.h"
#include "scene.h"
#include "playstage.h"
#include "Stage.h"
#include "introstage.h"
#include "Infostage.h"
#include "tutorialstage.h"
#include "pausestage.h"
#include "buystage.h"
#include "Player.h"

#include <cmath>

//some globals
Game* Game::instance = NULL;
FBO* fbo = NULL;

//stages and scene
std::map<std::string, Stage*> Stage::s_stages;
Stage* Stage::currentStage = NULL;
Scene* Scene::instance = NULL;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;
	free_cam = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//scene
	Scene::instance = new Scene();

	//stages
	Stage::currentStage = new Introstage();
	new Playstage();
	new Infostage();
	new Tutorialstage();
	new Pausestage();
	new Buystage();
	
	fbo = new FBO();
	fbo->create(window_width, window_height);
	
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

}


//what to do when the image has to be draw
void Game::render(void)
{
	if (Player::instance->fbo_active) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		fbo->bind();
	}
	

	//set the clear color (the background color)
	Vector3 sky_color = Scene::instance->skycolor;
	glClearColor(sky_color.x, sky_color.y, sky_color.z, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//rendering the current stage
	Stage::currentStage->render();

	if (Player::instance->fbo_active) {
		fbo->unbind();
		glDisable(GL_DEPTH_TEST);
		fbo->color_textures[0]->toViewport(Player::instance->fx);
	}
	
	//Draw the floor grid
	//drawGrid();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	Stage::currentStage->update(seconds_elapsed);

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_TAB: free_cam = !free_cam; break;
		case SDLK_F1: Shader::ReloadAll(); break; 
		//case SDLK_F3: Player::instance->fbo_active = !Player::instance->fbo_active; break;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

