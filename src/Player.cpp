#include "Player.h"
#include "Person.h"
#include "entity.h"
#include "entitymesh.h"
#include "input.h"
#include "scene.h"
#include "levelmap.h"
#include "game.h"
#include "camera.h"
#include "light.h"
#include "Mission_Person.h"
#include "Target_Person.h"
#include "Stage.h"
#include "Audio.h"
#include "shader.h"

Entitymesh* katana = NULL;
float yaw = 0;
Vector3 delta; Vector3 delta_attack;
Vector3 target;
Vector3 front;

bool back = false;
bool death = false; 
bool run = false;
bool movement;

Player::Player() : Person("data/Definitivo/main_character/ninja.mesh", "data/Definitivo/main_character/prueba2.tga") {

	instance = this;

	//adding player items
	katana = new Entitymesh("data/Definitivo/main_character/katana/source/katana2.obj", "data/Definitivo/main_character/katana/textures/katana.tga");

	pers_type = PLAYER;

	vista = false;

	//init stuff
	speed = 100;
	attacked = false;
	person.pos = Scene::instance->current_level->set_start_position(START);
	transform.translate(person.pos.x, 0, person.pos.z);
	name = "Player";
	stealth = false;
	
	current_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_idle2.skanim");
	next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_run.skanim");
	anim_mode = FRONT;

	num_enemies = die =  0;

	enter_mission = mission_mode = false;

	target_entity = NULL;

	//init fx features
	fbo_active = true;
	fx = Shader::Get("data/shaders/quad.vs", "data/shaders/effects_menu.fs");

}

Player::~Player(){

}

void Player::restartLife() {
	Player::instance->mission_mode = false; //return to free mode
	Player::instance->player_data.life = MAX_LIFE;
	Player::instance->die = 0;
	Player::instance->anim_mode = FRONT;
}

//in order to collect some items of the world (for now food or money)
void Player::collect() {
	//position of the player in Cell coordinates
	Vector2 position = Scene::instance->world2Cell(person.pos.x, person.pos.z); 

	//getting the cell of the map of these coordinates
	sCell& cell = Scene::instance->current_level->getCell((int)position.x, (int)position.y);

	//if the cell is colectable and there is collision between the objects in that cell
	if (cell.type == COIN || cell.type == FOOD && col) {

		col = false;

		for (int i = 0; i < near_entities.size(); i++) {
			Entitymesh*& ent = near_entities[i];

			eEntitymesh_type type = ent->meshtype;
			switch (type) {
			case MONEY:
				player_data.coins += COIN_VAL; //more money for the player
				Scene::instance->current_level->setCell((int)position.x, (int)position.y, EMPTY); //reset celltype
				Scene::instance->remove_entity(Scene::instance->entities, ent);
				Audio::manager->playSample("data/Definitivo/AudioSamples/coin_effect.mp3", false, true);
				break;
			case LIFE:
				player_data.life += FOOD_EXTRA; //more life for the player
				Scene::instance->current_level->setCell((int)position.x, (int)position.y, EMPTY);
				Scene::instance->remove_entity(Scene::instance->entities, ent);
				Audio::manager->playSample("data/Definitivo/AudioSamples/eat_effect.mp3", false, true);
				break;
			}
		}
	}
}

void Player::move_player(double dt, float dir=1.0) {

	if (vista) {
		person.speed = Vector3();
		person.speed_attack = Vector3();
		return;
	}

	delta = delta * dt * dir * speed;
	delta_attack = delta_attack * dt *50;

	person.speed = person.speed + delta.z * front;
	person.speed = person.speed - person.speed*0.05;

	person.speed_attack = person.speed_attack + delta_attack.z * Vector3(0,0,-1);
	person.speed_attack = person.speed_attack - person.speed_attack*0.05;

	target = target + person.speed*dt; //setting target position

	person.angle_speed += yaw * 3 * dt;
	person.angle_speed = person.angle_speed - person.angle_speed*0.06;
	person.angle = person.angle + person.angle_speed;
}

void Player::change_animation(e_Animation mode) {

	switch (mode) {
	case IDLE:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_idle2.skanim");
		break;
	case FRONT:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_run.skanim");
		break;
	case BACK:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_running_backward.skanim");
		break;
	case ATTACK:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_great_sword_slash.skanim");
		break;
	case DEATH:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_death.skanim");
		break;
	case STEALTH:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_sneak_walk.skanim");
		break;
	}
}


void Player::move_skeleton() {

	change_animation(anim_mode);

	float speed_factor = 0.0f;

	speed_factor = person.speed.length()*0.05;

	if (anim_mode == ATTACK || anim_mode == DEATH) {
		speed_factor = person.speed_attack.length()*0.05;
	}

	float run_factor = 20;

	current_anim->assignTime(Game::instance->time);
	float t = fmod(Game::instance->time, current_anim->duration) / current_anim->duration;

	if (run) {
		run_factor *= 2;
	}

	if (anim_mode == DEATH) {

		//to control that only die once
		if (die > next_anim->duration - 0.1)
			return;

		next_anim->assignTime(die);
		final_skeleton = next_anim->skeleton;
		return;
	}
	else {
		next_anim->assignTime(t*next_anim->duration*run_factor);
	}

	blendSkeleton(&current_anim->skeleton, &next_anim->skeleton, speed_factor, &final_skeleton);

	final_skeleton.updateGlobalMatrices();

}

void Player::render_extras(Camera* camera, Light* light) {
	Matrix44 hand = final_skeleton.getBoneMatrix("mixamorig_RightHandThumb1", false);
	hand = hand * transform;

	hand.rotate(180 * DEG2RAD, Vector3(1, 1, 1));

	katana->shader->enable();

	katana->render_basic_shader(camera, hand, light);
	katana->mesh->render(GL_TRIANGLES, -1);

	katana->shader->disable();
}

void Player::changeTargetentity(eMissionState mission_state) {
	switch (mission_state) {
	case INITIAL:
		target_entity = Mission_Person::instance;
		break;
	case INFORMANTS:
		target_entity = Mission_Person::instance->informant;
		break;
	case LETSFINISH:
		target_entity = Target_Person::instance;
		break;
	}
}

void Player::behaviourtree(double dt) {
	if (player_data.life <= 0) {
		anim_mode = DEATH;
		Vector3 delta_attack = delta_attack * dt * 1000;
		person.speed_attack = person.speed_attack + delta_attack;
		person.speed_attack = person.speed_attack - person.speed_attack*0.05;
		die += dt;
		return;
	}

	changeTargetentity(Mission_Person::instance->missionState);

	if (target_entity == NULL) {

		dist_to_player = 0;
		std::cout << "No target" << std::endl;

	}
	else {
		//getting current position
		Vector3 this_pos = transform.getTranslation();
		//getting position of the mission person
		Vector3 mission_pos = target_entity->transform.getTranslation();
		dist_to_player = this_pos.distance(mission_pos);
	}

	collect();

	
}

void Player::periodic_audio(const char* filename, float speed, int period) {
	int mod = Game::instance->frame % period;
	if (speed > 2 && mod == 0) {
		Audio::manager->playSample(filename, false, true);
	}
}

void Player::update(double dt) {

	//init bools
	col = false; run = false; attack = false; movement = false;
	HATState P_hat = Input::gamepads[0].hat;//hat of the gamepad

	Vector3 mov_speed = Vector3(0, 0, 8); //speed of movement

	rot.setRotation(person.angle*DEG2RAD, Vector3(0, 1, 0));
	front = rot.rotateVector(Vector3(0, 0, -1)); //rotate front vector

	//Scene::instance->main_light->position = Vector3(person.pos.x, 5, person.pos.z);

	target = person.pos;
	delta = delta_attack = Vector3(); yaw = 0; //init speeds and angles

	//movement
	/********************************************************************************************/
	if (Input::isKeyPressed(SDL_SCANCODE_UP) || P_hat == PAD_UP) { 
		delta = delta - mov_speed;
		if (stealth) { 
			anim_mode = STEALTH;
		}
		else {
			anim_mode = FRONT;
		}
		movement = true; 
	}
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN) || P_hat == PAD_DOWN) { delta = delta + mov_speed; anim_mode = BACK; movement = true;}
	if (Input::isKeyPressed(SDL_SCANCODE_LEFT) || P_hat == PAD_LEFT) yaw = yaw - 180 * dt;
	if (Input::isKeyPressed(SDL_SCANCODE_RIGHT) || P_hat == PAD_RIGHT) yaw = yaw + 180 * dt;

	//cases of the hat not compatible with cursors
	switch (P_hat) {
	case PAD_RIGHTUP:
		delta = delta - mov_speed;
		if (!stealth)
			anim_mode = FRONT;
		else
			anim_mode = STEALTH; 

		movement = true;
		yaw = yaw + 180 * dt;
		break;
	case PAD_RIGHTDOWN:
		delta = delta + mov_speed; anim_mode = BACK; movement = true;
		yaw = yaw + 180 * dt;
		break;
	case PAD_LEFTUP:
		delta = delta - mov_speed; 
		
		if (!stealth)
			anim_mode = FRONT;
		else
			anim_mode = STEALTH;
		
		movement = true;
		yaw = yaw - 180 * dt;
		break;
	case PAD_LEFTDOWN:
		delta = delta + mov_speed; anim_mode = BACK; movement = true;
		yaw = yaw - 180 * dt;
		break;
	}
	/********************************************************************************************/

	//buttons
	/********************************************************************************************/
	//attack
	if (Input::isKeyPressed(SDL_SCANCODE_Z) || Input::isButtonPressed(X_BUTTON)) { 
		if (!movement) {
			delta_attack = delta_attack + mov_speed;
			anim_mode = ATTACK; attack = true;
		}
	}
	//enter missions
	if (Input::wasKeyPressed(SDL_SCANCODE_X) || Input::wasButtonPressed(B_BUTTON)) {
		if (dist_to_player < 30) {
			enter_mission = true;
			//std::cout << enter_mission << std::endl;
		}
	}
	//pause menu
	if (Input::isKeyPressed(SDL_SCANCODE_P) || Input::wasButtonPressed(A_BUTTON)) {
		//stop playstage audio
		Audio::manager->stopChannel("data/Definitivo/AudioSamples/Wind_backgroundSound.mp3");
		//play menu/pausemenu audio
		Audio::manager->playSample("data/Definitivo/AudioSamples/musica_epica_fondo_menu.mp3", true, false);
		Stage::currentStage->changeStage("pause");
	}
	//stealth_mode
	if (Input::wasKeyPressed(SDL_SCANCODE_S) || Input::wasButtonPressed(LB_BUTTON)) {
		stealth = !stealth;
		if(stealth)
			Audio::manager->playSample("data/Definitivo/AudioSamples/stealth_effect.mp3", false, true);
	}

	//speed sound effect
	if (Input::wasKeyPressed(SDL_SCANCODE_C) && !stealth || Input::wasButtonPressed(RB_BUTTON) && !stealth) {
		Audio::manager->playSample("data/Definitivo/AudioSamples/run_effect.mp3", false, true);
	}
	//speed change
	if (Input::isKeyPressed(SDL_SCANCODE_C) && !stealth || Input::isButtonPressed(RB_BUTTON) && !stealth) {
		run = true;
		speed = 500;
	}
	else if (stealth) {
		speed = 50;
	}
	else {
		speed = 100;
	}
	
	if (run) {
		//run fx
		fbo_active = true;
		fx = Shader::Get("data/shaders/quad.vs", "data/shaders/effects_run.fs");
	}
	else if (stealth) {
		//stealth fx
		fbo_active = true;
		fx = Shader::Get("data/shaders/quad.vs", "data/shaders/effects_stealth.fs");
	}
	else {
		//deactivate fx
		fbo_active = false;
	}



	
	/********************************************************************************************/

	//movement audio
	if(!stealth && !run)
		periodic_audio("data/Definitivo/AudioSamples/NinjaWalking_Normal.mp3", person.speed.length(),60);
	


	//adding physics
	if (!attacked) {
		move_player(dt);
	}
	else {
		move_player(dt, -5.0f);
	}

	if (player_data.life <= 0) {
		person.speed = Vector3();
		person.speed_attack = Vector3();
		person.angle = 0;
	}
	
	//collision
	Vector3 coll; Vector3 norm;
	/*in order to optimize the collisions we can avoid testing with all the entites of the world (only testing 
	the collision with the big and near entities(in an area of 30 units))*/
	near_entities = Scene::instance->near_ent(transform, 30);
	collision(target, coll, norm, near_entities, dt);

	if (col)
		person.speed = Vector3();//if colision do not move(animation stops)

	//if player is inside the limits of the world
	if (target.x > 50 && target.z > 50 && target.x < 5070 && target.z < 5070 && !col) {
		person.pos = target; //setting the position
	}
	
	behaviourtree(dt);

	//setting rotation and traslation of the player
	transform.setTranslation(person.pos.x, person.pos.y, person.pos.z);
	transform.rotate(person.angle*DEG2RAD, Vector3(0, 1, 0));

	//DEBUG
	/***********************************************************************************/
	if (Input::wasKeyPressed(SDL_SCANCODE_1)) {
		player_data.coins += 900;
	}

	if (Input::isKeyPressed(SDL_SCANCODE_2)) {
		player_data.life = -1;
	}

	/*if (Input::isKeyPressed(SDL_SCANCODE_3)) {
		float g = Target_Person::instance->life;
		std::cout << "Vida Target: "<< g << std::endl;
	}*/
	/***********************************************************************************/
	

}