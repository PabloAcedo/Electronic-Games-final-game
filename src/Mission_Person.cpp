#include "Mission_Person.h"
#include "entity.h"
#include "entitymesh.h"
#include "Person.h"
#include "game.h"
#include "Player.h"
#include "Stage.h"
#include "input.h"
#include "Target_Person.h"
#include "includes.h"
#include "Audio.h"
#include "shader.h"

#define REWARD 60; //and also the price for buying items

//not used yet, but will be used in order to save the game data
struct sGame_Data {
	int current_level;
	sPlayer playerData;
	sPerson playerPersonalData;
};

Entitymesh* hat = NULL; float wintime = 0;

Mission_Person::Mission_Person(): Person("data/Definitivo/main_character/ninja.mesh", "data/Definitivo/Persons_and_enemies/japan_peasant.tga") {

	instance = this;

	speed = 60;
	hat = new Entitymesh("data/Definitivo/main_character/Accesorios/japan_hat.obj", "data/Definitivo/main_character/Accesorios/japan_hat.png");
	hat->transform.translate(0, -2, 0);
	anim_mode = FRONT;


	person.pos = Scene::instance->current_level->set_start_position(MISSION);
	transform.translate(person.pos.x, 0, person.pos.z);

	levelcount = informantcount = informantNum = 0;
	missionState = INITIAL;

}


Mission_Person::~Mission_Person(){



}

void Mission_Person::move_skeleton() {

	change_animation(anim_mode);

	float speed_factor = 0.0f;

	speed_factor = person.speed.length()*0.05;

	if (anim_mode == HELLO) {
		speed_factor = person.speed_attack.length()*0.05;
	}

	float run_factor = 20;

	current_anim->assignTime(Game::instance->time);
	float t = fmod(Game::instance->time, current_anim->duration) / current_anim->duration;

	next_anim->assignTime(t*next_anim->duration*run_factor/3);

	blendSkeleton(&current_anim->skeleton, &next_anim->skeleton, speed_factor, &final_skeleton);

	final_skeleton.updateGlobalMatrices();

}

void Mission_Person::change_message(const char* &message) {

	if (informantcount == 2) {
		message = "Level Completed.\n\n\nPress X";
		return;
	}else if (informantcount == 3) {
		message = "DEAD!\n\nPress X";
		return;
	}

	switch (levelcount) {
	case 0:
		if (informantcount == 0) {
			message = "Your mission:\nSomeone is robbing.\nFirst look for info, then...\nKILL THE TARGET!!\n";
		}
		else if (informantcount == 1) {
			message = "I've seen the bad guy\nin the forest.";
		}
		break;
	case 1:
		if (informantcount == 0) {
			message = "Your mission:\nI have a job.\nGo to the guy, he will tell you what to do.\n";
		}
		else if (informantcount == 1) {
			message = "The General is robbing us...\nKill him.";
		}
		break;
	default:
		message = "Sorry\nNo more levels!!:)\nPress X";
		Player::instance->mission_mode = false;
		missionState = GAMEFINISHED;
		break;
	}

}

void Mission_Person::buy_things(eBuyItem item) {
	int balance = Player::instance->player_data.coins - REWARD;
	//if the balance is positive, increment player life
	if (balance >= 0) {
		Player::instance->player_data.coins -= REWARD;

		if (item == STRENGTH) {
			//increment damage by 5
			Player::instance->player_data.damage += 5;
		}
		else if (item == LIFES) {
			//increment player life
			Player::instance->player_data.life += MAX_LIFE;
		}

	}
	
}



void Mission_Person::restart_all() {
	//restart all the parameters of the player
	Player::instance->restartLife();
	Player::instance->player_data.damage = 5;
	Player::instance->player_data.coins = 0;
	Player::instance->person.pos = Scene::instance->current_level->set_start_position(START);
	Player::instance->transform.translate(Player::instance->person.pos.x, 0, Player::instance->person.pos.z);
	levelcount = informantcount = informantNum = 0;
	missionState = INITIAL;
}

void Mission_Person::saveGame() {

	if (Player::instance->mission_mode) {
		std::cout << "You cannot save, you are in a mission." << std::endl;
		return;
	}

	sGame_Data game_info;

	//storing the data

	//storing level number
	game_info.current_level = levelcount;
	//storing player main data
	game_info.playerData = Player::instance->player_data;
	game_info.playerPersonalData = Player::instance->person;

	//storing in file
	FILE* fp = fopen("data/Definitivo/GameData/savegame.bin", "wb");
	fwrite(&game_info, sizeof(sGame_Data), 1, fp);
	fclose(fp);
	std::cout << "Saved." << std::endl;

}

bool Mission_Person::loadGame() {

	if (Player::instance->mission_mode) {
		std::cout << "You cannot load a game, you are in a mission." << std::endl;
		return false;
	}

	sGame_Data game_info;
	//open file
	FILE* fp = fopen("data/Definitivo/GameData/savegame.bin", "rb");

	if (fp == NULL) {
		std::cout << "Cannot find the game file." << std::endl;
		return false;
	}
	fread(&game_info, sizeof(sGame_Data), 1, fp);
	fclose(fp);

	
	levelcount = game_info.current_level;//setting level number

	//player data and position/personal_data
	Player::instance->player_data = game_info.playerData;
	Player::instance->person = game_info.playerPersonalData;
	Player::instance->transform.translate(Player::instance->person.pos.x, 0, Player::instance->person.pos.z);

	std::cout << "Game loaded." << std::endl;

	return true;
}


void Mission_Person::render_extras(Camera* camera, Light* light) {
	Matrix44 head = final_skeleton.getBoneMatrix("mixamorig_HeadTop_End", false);
	head = head * transform;

	hat->shader->enable();

	hat->render_basic_shader(camera, head, light);
	hat->mesh->render(GL_TRIANGLES, -1);

	hat->shader->disable();
}

void Mission_Person::behaviour_tree(Vector3 front, double dt) {
	Vector3 this_pos = transform.getTranslation();//getting position of the current person
	Vector3 player_pos = Player::instance->transform.getTranslation(); //getting position of the player

	dist_to_player = this_pos.distance(player_pos);//computing distance to the player

	Vector3 to_target = this_pos - player_pos;

	rotate_person(to_target, front, dt);

	if (dist_to_player < 200  && dist_to_player > 50) {

		//say hello to the player (use the speed_attack to move the animation)
		if (!Player::instance->mission_mode) {
			anim_mode = HELLO;
			Vector3 delta_attack = delta_attack * dt * 1000;
			person.speed_attack = person.speed_attack + delta_attack;
			person.speed_attack = person.speed_attack - person.speed_attack*0.05;
		}
		
			
	} else {
		//speed of animation = 0
		person.speed_attack = person.speed_attack - person.speed_attack*0.05;
	}
	

	if (!Player::instance->mission_mode) {

		if (Player::instance->player_data.life <= 0) {
			missionState = DIENOTLEVEL;
			informantcount = 3;
			//player death effect
			Audio::manager->playSample("data/Definitivo/AudioSamples/player_death.mp3", false, true);
			Stage::currentStage->changeStage("info");
		}

		if (dist_to_player < 30 && missionState == INITIAL || missionState == GAMEFINISHED) {
			//player click the button in order to enter the mission
			if (Player::instance->enter_mission && informantcount == 0) { 
				//if the target stills in the entities list, remove it
				Scene::instance->remove_entity(Scene::instance->entities, Target_Person::instance);
				Player::instance->vista = true; //change the camera posiion to speak mode
				Scene::instance->begin_level(levelcount); //load the current level
				//speeds to 0
				person.speed_attack = person.speed_attack - person.speed_attack*0.05;
				person.speed = Vector3();
				Player::instance->person.speed = Player::instance->person.speed_attack = Vector3();
				//enter mission sound effect
				Audio::manager->playSample("data/Definitivo/AudioSamples/begin_mission.mp3", false, true);
				//change to the info stage in order to see the mission goal
				Stage::currentStage->changeStage("info");
			}
		}
	}else if (Player::instance->mission_mode) {

			if (Player::instance->player_data.life <= 0) {
				missionState = GAMEOVERR;
				informantcount = 3;
				//player death effect
				Audio::manager->playSample("data/Definitivo/AudioSamples/player_death.mp3", false, true);
				Stage::currentStage->changeStage("info");
			}

			if (missionState == INFORMANTS && Player::instance->enter_mission) {
				//talk to informant
				Player::instance->vista = true; //change the camera position to speak mode
				Player::instance->person.speed = Player::instance->person.speed_attack = Vector3();
				//informant info sound
				Audio::manager->playSample("data/Definitivo/AudioSamples/info_sound.mp3", false, true);
				//change to the info stage in order to see the mission goal
				Stage::currentStage->changeStage("info");

			} else if (missionState == LETSFINISH) {
					//when player kills the target, mission accomplished
				if (Target_Person::instance->life <= 0) {
					informantcount = 2;
					wintime += dt; //time to let the target die visually
					if (wintime > 4) {
						wintime = 0;
						//efects, sound and go to infostage
						Player::instance->fbo_active = true; 
						Player::instance->fx = Shader::Get("data/shaders/quad.vs", "data/shaders/effects_grayscale.fs");
						Audio::manager->playSample("data/Definitivo/AudioSamples/mission_completed.mp3", false, true);
						Stage::currentStage->changeStage("info");
					}
				}
			}
		}

}

void Mission_Person::to_nextlevel() {

	//delete the entities of this level
	Scene::instance->clear_level();

	//progress only if win
	if (missionState != GAMEOVERR) {
		Player::instance->player_data.coins += REWARD;
		levelcount += 1;
	}

	Player::instance->mission_mode = false;
	informantcount = 0;
}

void Mission_Person::abandonMission() {

	if (!Player::instance->mission_mode) {
		std::cout << "You are not in a mission." << std::endl;
		return;
	}

	missionState = GAMEOVERR;
	to_nextlevel();
	missionState = INITIAL;
	Scene::instance->remove_entity(Scene::instance->entities, Target_Person::instance);
	Scene::instance->change_level(Mission_Person::instance->levelcount);//reset level (restarting target life)
}

void Mission_Person::update(double dt) {

	rot.setRotation(person.angle*DEG2RAD, Vector3(0, 1, 0));
	Vector3 front = rot.rotateVector(Vector3(0, 0, -1));

	behaviour_tree(front, dt);

	transform.setTranslation(person.pos.x, person.pos.y, person.pos.z);
	transform.rotate(person.angle*DEG2RAD, Vector3(0, 1, 0));
}