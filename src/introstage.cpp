#include "Stage.h"
#include "introstage.h"
#include "playstage.h"
#include "game.h"
#include "scene.h"
#include "input.h"
#include "framework.h"
#include "Audio.h"
#include "Player.h"

Introstage::Introstage(const char* intro, const char* menu_sprite) : Stage("intro") {

	mode = PLAY;

}

Introstage::Introstage() : Stage("intro"){
	mode = PLAY;
	Scene::instance->update(1);
	buttons_text = "data/Definitivo/gui/menu_buttons.png";
	Audio::manager->playSample("data/Definitivo/AudioSamples/musica_epica_fondo_menu.mp3", true, false);
}


void Introstage::render() {

	//BG
	Scene::instance->renderGUI(Game::instance->window_width / 2, Game::instance->window_height / 2,
		Game::instance->window_width, Game::instance->window_height,
		"data/Definitivo/gui/Game_Menu.png", Vector4(0,0,1,1));

	//Buttons
	for (int i = 0; i < 5; i++) {
		Scene::instance->renderButton(Game::instance->window_width / 2, button_coord[i],
			but_sizex, but_sizey,
			buttons_text, Vector4(0, 0.1*sprite_coord[i], 1, 0.11), options[i]);
	}

}



void Introstage::update(double seconds_elapsed) {

	//menu fx
	Player::instance->fbo_active = true;
	Player::instance->fx = Shader::Get("data/shaders/quad.vs", "data/shaders/effects_menu.fs");

	update_hat_time(seconds_elapsed);

	//using event_control instead of directly the mode because of different modes in pause menu and intro menu
	//update of button
	change_event(5);
	
	//changing mode
	switch (event_control) {
	case 0:
		mode = PLAY; break;
	case 1:
		mode = LOAD; break;
	case 2:
		mode = CONTROLS; break;
	case 3:
		mode = RESTART; break;
	case 4:
		mode = QUIT; break;
	}

	//entering on the different options of the menu
	if (Input::wasKeyPressed(SDL_SCANCODE_X) || Input::wasButtonPressed(B_BUTTON)) {
		//play select audio effect
		Audio::manager->playSample("data/Definitivo/AudioSamples/stealth_attack.mp3", false, true);
		Stage::currentStage->Select_in_menu();
	}

}

