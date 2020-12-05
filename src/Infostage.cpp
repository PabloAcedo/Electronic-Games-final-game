#include "Infostage.h"
#include "Stage.h"
#include "scene.h"
#include "Player.h"
#include "Mission_Person.h"
#include "input.h"
#include "Target_Person.h"
#include "Person.h"
#include "game.h"



Infostage::Infostage() : Stage("info") {
	mode = INFO;
}


Infostage::~Infostage(){

}

void Infostage::render() {
	if (Player::instance->player_data.life > 0 && Mission_Person::instance->missionState != LETSFINISH) {
		Scene::instance->render(20);
		drawText(50, 400, message, Vector3(1, 1, 1), 3);
	}
	else  if (Mission_Person::instance->missionState == LETSFINISH) {
		Scene::instance->render(20);
		Scene::instance->renderGUI(Game::instance->window_width / 2, Game::instance->window_height / 2,
			Game::instance->window_width, Game::instance->window_height,
			"data/Definitivo/gui/misionpasada_pantalla.png", Vector4(0, 0, 1, 1));
	}
	else {
		Scene::instance->render(20);
		Scene::instance->renderGUI(Game::instance->window_width / 2, Game::instance->window_height / 2,
			Game::instance->window_width, Game::instance->window_height,
			"data/Definitivo/gui/dead_pantalla.png", Vector4(0, 0, 1, 1));
	}
	

}

void  Infostage::update(double seconds_elapsed) {

	//to see the animation of player death
	if(Player::instance->player_data.life <= 0)
		Player::instance->update(seconds_elapsed);

	Mission_Person::instance->change_message(message);

	if (Input::wasKeyPressed(SDL_SCANCODE_X) || Input::wasButtonPressed(B_BUTTON)) {

		Player::instance->vista = false;
		Player::instance->enter_mission = false;
		
		if (Mission_Person::instance->missionState == INITIAL) {
			std::cout << "To Informant" << std::endl; //debug
			Mission_Person::instance->missionState = INFORMANTS;
			Mission_Person::instance->informantcount += 1;
			Player::instance->mission_mode = true;

		}else if (Mission_Person::instance->missionState == INFORMANTS) {
			std::cout << "Lets finish this" << std::endl; //debug
			Mission_Person::instance->missionState = LETSFINISH;
			Scene::instance->add_entity(Scene::instance->entities, Target_Person::instance);
			Mission_Person::instance->informantcount += 1;
			Player::instance->mission_mode = true;
		}
		else if (Mission_Person::instance->missionState == LETSFINISH) {
			Mission_Person::instance->to_nextlevel();//update to next level
			Mission_Person::instance->missionState = INITIAL;

		}else if (Mission_Person::instance->missionState == GAMEOVERR) {
			std::cout << "DEAD" << std::endl; //debug
			Mission_Person::instance->abandonMission(); //aborting mission
			//restart player life
			Player::instance->restartLife();

		}else if (Mission_Person::instance->missionState == DIENOTLEVEL) {
			std::cout << "DEAD" << std::endl; //debug

			//restart player life
			Player::instance->restartLife();

			//por si acaso
			Mission_Person::instance->missionState = INITIAL;
			Mission_Person::instance->informantcount = 0;
		}

		Player::instance->fbo_active = false;
		Stage::currentStage->changeStage("play");
	}

}
