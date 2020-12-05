#include "buystage.h"
#include "Player.h"
#include "game.h"
#include "input.h"
#include "Audio.h"


Buystage::Buystage() : Stage("buy"){

	buttons_text = "data/Definitivo/gui/store_buttons.png";
	mode = BUY_STRENGTH;
}


Buystage::~Buystage(){

}

void Buystage::render() {
	Vector4 icon_atlas[] = { {0,0,0.2,0.2}, {0.2,0,0.4,0.4} };

	//BG
	Scene::instance->renderGUI(Game::instance->window_width / 2, Game::instance->window_height / 2,
		Game::instance->window_width, Game::instance->window_height,
		"data/Definitivo/gui/Store_menu.png", Vector4(0, 0, 1, 1));

	//Buttons
	for (int i = 0; i < 2; i++) {
		Scene::instance->renderButton(Game::instance->window_width / 2, button_coord[i],
			but_sizex, but_sizey,
			buttons_text, Vector4(0, 0.1*sprite_coord[i+1], 1, 0.11), options[i]);
	}

	if (Player::instance->player_data.coins < 60) {
		float y = sin(1.5 * PI * Game::instance->time);
		if (y > 0) {
			Scene::instance->renderGUI(Game::instance->window_width / 2, 456,
				but_sizex, but_sizey,
				buttons_text, Vector4(0, 0.1*sprite_coord[3], 1, 0.11));
		}
	}

	//Coin icon
	Scene::instance->renderGUI(670, 500,
		70, 70,
		"data/Definitivo/gui/icon_atlas.png", icon_atlas[0]);

	//coins
	drawText(700, 500, std::to_string(Player::instance->player_data.coins), Vector3(1, 1, 1), 2);
}

void Buystage::update(double seconds_elapsed) {

	update_hat_time(seconds_elapsed);
	change_event(2);

	//changing mode
	switch (event_control) {
	case 0:
		mode = BUY_STRENGTH; break;
	case 1:
		mode = BUY_LIFE; break;
	}

	//entering on the different options of the menu
	if (Input::wasKeyPressed(SDL_SCANCODE_X) || Input::wasButtonPressed(B_BUTTON)) {
		Audio::manager->playSample("data/Definitivo/AudioSamples/coin_effect.mp3", false, true);
		Stage::currentStage->Select_in_menu();
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_Z) || Input::wasButtonPressed(X_BUTTON)) {
		Stage::currentStage->changeStage("pause");
	}

}
