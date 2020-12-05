#include "pausestage.h"
#include "Stage.h"
#include "game.h"
#include "scene.h"
#include "Player.h"
#include "input.h"
#include "Audio.h"

Pausestage::Pausestage() : Stage("pause"){
	mode = PLAY;
	buttons_text = "data/Definitivo/gui/pause_menu_buttons.png";
}


Pausestage::~Pausestage(){

}

void Pausestage::render() {

	//BG
	Scene::instance->renderGUI(Game::instance->window_width / 2, Game::instance->window_height / 2,
		Game::instance->window_width, Game::instance->window_height,
		"data/Definitivo/gui/Game_pause_Menu.png", Vector4(0, 0, 1, 1));

	//Buttons
	for (int i = 0; i < 6; i++) {
		Scene::instance->renderButton(Game::instance->window_width / 2, button_coord[i],
			but_sizex, but_sizey,
			buttons_text, Vector4(0, 0.1*sprite_coord[i], 1, 0.11), options[i]);
	}

}

void Pausestage::update(double seconds_elapsed) {
	//activate menu fx
	Player::instance->fbo_active = true;
	Player::instance->fx = Shader::Get("data/shaders/quad.vs", "data/shaders/effects_menu.fs");

	update_hat_time(seconds_elapsed);

	//using event_control instead of directly the mode because of different modes in pause menu and intro menu
	//update of button
	change_event(6);

	//changing mode
	switch (event_control) {
	case 0:
		mode = PLAY; break;
	case 1:
		mode = SAVE; break;
	case 2:
		mode = CONTROLS; break;
	case 3:
		mode = BUY; break;
	case 4:
		mode = MAIN; break;
	case 5:
		mode = ABANDON_MISSION; break;
	}

	//entering on the different options of the menu
	if (Input::wasKeyPressed(SDL_SCANCODE_X) || Input::wasButtonPressed(B_BUTTON)) {
		//play select audio effect
		Audio::manager->playSample("data/Definitivo/AudioSamples/stealth_attack.mp3", false, true);
		Stage::currentStage->Select_in_menu();
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_Z) || Input::wasButtonPressed(X_BUTTON)) {
		//deactivate fx
		Player::instance->fbo_active = true;
		//play select audio effect
		Audio::manager->playSample("data/Definitivo/AudioSamples/stealth_attack.mp3", false, true);
		mode = PLAY;
		Stage::currentStage->Select_in_menu();
	}

}