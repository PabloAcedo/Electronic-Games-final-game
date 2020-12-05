
#include "Stage.h"
#include "input.h"
#include "scene.h"
#include "game.h"
#include "Mission_Person.h"
#include "Player.h"
#include "Audio.h"

int _button_coord[] = { {200}, {264}, {328}, {392}, {456}, {520} };


Stage::Stage() {
	hat_time = 0;
}

Stage::Stage(const char* stage_type) {
	s_stages[stage_type] = this;
	message = "Hello buddy";
	hat_time = 0;

	//init menu things
	if (stage_type == "pause" || stage_type == "intro" || stage_type == "buy") {
		event_control = 0;
		but_sizex = 400; but_sizey = 50;

		for (int i = 0; i < 6; i++) {
			sprite_coord[i] = i;
		}

		for (int i = 0; i < 6; i++) {
			button_coord[i] = _button_coord[i];
		}

		for (int i = 0; i < 6; i++) {
			if (i == 0) {
				options[i] = true;
			}
			else {
				options[i] = false;
			}
		}
	}

}

//controlling the hat last pressed button by the time that is pressed
bool Stage::input_plus_time(int pad_button) {
	if (Input::gamepads[0].hat == pad_button && hat_time > 0.023) {
		return true;
	}
	return false;
}


void Stage::change_event(int events_num) {
	if (event_control >= 0 && event_control <= events_num-1) {
		if (Input::wasKeyPressed(SDL_SCANCODE_DOWN) || input_plus_time(PAD_DOWN)) {

			int opt = event_control + 1;

			if (opt < events_num)
				event_control += 1;

		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_UP) || input_plus_time(PAD_UP)) {
			int opt = event_control - 1;

			if (opt > -1)
				event_control -= 1;
		}

		//visual update
		for (int i = 0; i < events_num; i++) {
			if (i == event_control) {
				options[i] = true;
			}
			else {
				options[i] = false;
			}
		}
	}
}

//controlling the hat last pressed button by the time that is pressed
void Stage::update_hat_time(double seconds_elapsed) {
	//if the hat is pressed
	if (Input::wasHatPressed()) {
		Scene::instance->gamePad = true;
		//compute the response time in order to change between menu options
		hat_time += seconds_elapsed;
		if (hat_time >= 0.024) {
			hat_time = 0;
		}
	}
	else {
		hat_time = 0;
	}
}

void Stage::Select_in_menu() {
	switch (mode) {
	case PLAY:
		//stop fx
		Player::instance->fbo_active = false;
		//stop audio from menu
		Audio::manager->stopChannel("data/Definitivo/AudioSamples/musica_epica_fondo_menu.mp3");
		//play playstage audio
		Audio::manager->playSample("data/Definitivo/AudioSamples/Wind_backgroundSound.mp3", true, false);
		Stage::currentStage->changeStage("play");
		break;
	case LOAD:
		Mission_Person::instance->loadGame();
		break;
	case RESTART:
		if (!Player::instance->mission_mode) {
			Mission_Person::instance->restart_all();
		}
		else {
			std::cout << "You are in a mission, you cannot restart." << std::endl;
		}
		break;
	case SAVE:
		Mission_Person::instance->saveGame();
		break;
	case BUY:
		Stage::currentStage->changeStage("buy");
		break;
	case BUY_STRENGTH:
		Mission_Person::instance->buy_things(STRENGTH);
		break;
	case BUY_LIFE:
		Mission_Person::instance->buy_things(LIFES);
		break;
	case MAIN:
		Stage::currentStage->changeStage("intro");
		break;
	case CONTROLS:
		Stage::currentStage->changeStage("tutorial");
		break;
	case ABANDON_MISSION:
		Mission_Person::instance->abandonMission();
		break;
	case QUIT:
		Game::instance->must_exit = true; break;
	}

}

void Stage::render() {}
void Stage::update(double seconds_elapsed) {}

void Stage::changeStage(const char* stage_type) {
	currentStage = s_stages[stage_type];
}