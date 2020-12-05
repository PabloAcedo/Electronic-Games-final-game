
#include "playstage.h"
#include "input.h"
#include "includes.h"
#include "Stage.h"
#include "game.h"
#include "scene.h"
#include "Player.h"
#include "Target_Person.h"


Playstage::Playstage() : Stage("play") {
	mode = PLAY;
}


Playstage::~Playstage(){
	 
}

void Playstage::render(){
	Vector4 icon_atlas[] = { {0,0,0.2,0.2}, {0.2,0,0.4,0.4}, {0.6,0,0.4,0.4} };

	Scene::instance->render(10);

	Vector3 color = Vector3(1, 1, 1);

	
	if (Player::instance->dist_to_player < 30) {
		//can enter mission
		float y = sin(2 * PI * Game::instance->time);
		if(y > 0 && Player::instance->target_entity != Target_Person::instance)
			drawText(100, 500, "Press X", color, 4);

	}

	//coins
	drawText(700, 500, std::to_string(Player::instance->player_data.coins), color, 2);

	//Coin icon
	Scene::instance->renderGUI(670, 500,
		70, 70,
		"data/Definitivo/gui/icon_atlas.png", icon_atlas[0]);

	//coins
	drawText(700, 500, std::to_string(Player::instance->player_data.coins), color, 2);

	//Coin icon
	Scene::instance->renderGUI(680, 580,
		70, 70,
		"data/Definitivo/gui/icon_atlas.png", icon_atlas[1]);

	//dist 2 target
	drawText(700, 555, std::to_string((int)Player::instance->dist_to_player), color, 2);

	//Life icon
	Scene::instance->renderGUI(660, 75,
		70, 70,
		"data/Definitivo/gui/icon_atlas.png", icon_atlas[2]);

	//Life in %
	drawText(700, 50, std::to_string((int)Player::instance->player_data.life)+"%", color, 2);

}


void Playstage::update(double seconds_elapsed) {

	Scene::instance->update(seconds_elapsed);
	mode = PLAY;
}