#include "Enemy.h"
#include "Player.h"
#include "scene.h"
#include "game.h"
#include "entitymesh.h"
#include "Audio.h"

Entitymesh* sword; float rot_time = 0.0f; float r_angle;

Enemy::Enemy() : Person("data/Definitivo/main_character/ninja.mesh", "data/Definitivo/Persons_and_enemies/japan_soldier.tga"){
	//initialize 
	speed = 60;
	life = 100;
	damage = 0.3;
	walk = false;
	sword = new Entitymesh("data/Definitivo/main_character/katana/source/katana2.obj", "data/Definitivo/main_character/katana/textures/katana.tga");
	die = 0;
	stop = false;
	person.angle += random() * 360;
	transform.rotate(person.angle*DEG2RAD, Vector3(0, 1, 0));
}


Enemy::~Enemy(){

}

void Enemy::render_extras(Camera* camera, Light* light) {
	//rendering the katana in the hand
	Matrix44 hand = final_skeleton.getBoneMatrix("mixamorig_RightHandThumb1", false);
	hand = hand * transform;

	hand.rotate(180 * DEG2RAD, Vector3(1, 1, 1));

	sword->shader->enable();

	sword->render_basic_shader(camera, hand, light);
	sword->mesh->render(GL_TRIANGLES, -1);

	sword->shader->disable();
}

bool Enemy::can_see(Vector3 front, Vector3 to_target) {

	front.normalize(); to_target.normalize();

	float angle = front.dot(to_target);

	//the enemy can detect the player if this one is not in stealth mode
	if (dist_to_player < 20 && Player::instance->stealth == false) {
		return true;
	}

	//see if it is in the angle of view
	if (angle > 0.6) {
		return true;
	}

	return false;

}

void Enemy::move_enemy(Vector3 front, double dt, float dir = 1.0) {

	Vector3 delta;

	//updating enemy speed
	delta = delta + dir*Vector3(0, 0, -8);
	delta = delta * dt*speed;
	person.speed = person.speed + delta.z * front;
	person.speed = person.speed - person.speed*0.05;
	person.pos = person.pos + person.speed*dt;

	anim_mode = FRONT; //run animation
}

void Enemy::go_to_target(Vector3 to_target, Vector3 front, double dt) {
	//move to the target vector
	if (person.pos.x != to_target.x && person.pos.z != to_target.z) {
		move_enemy(front, dt);
	}
}

void Enemy::attack_player(Vector3 front, float dt) {

	anim_mode = ATTACK;
	//attack velocity
	Vector3 delta_attack = delta_attack * dt * 1000;
	person.speed_attack = person.speed_attack + delta_attack;
	person.speed_attack = person.speed_attack - person.speed_attack*0.05;

	Player::instance->player_data.life -= damage;
	person.speed = Vector3(); //if the enemy is attacking, he doesnt move

}



void Enemy::move_skeleton() {

	change_animation(anim_mode); //changing type of next animation

	float speed_factor = 0.0f;

	speed_factor = person.speed.length()*0.05;

	if (anim_mode == ATTACK || anim_mode == DEATH) {
		speed_factor = person.speed_attack.length()*0.05;
	}

	float run_factor = 20;

	current_anim->assignTime(Game::instance->time);
	float t = fmod(Game::instance->time, current_anim->duration) / current_anim->duration;

	//if enemy dies, always do this animation
	if (anim_mode == DEATH) {

		//to control that the enemy only die once
		if (die > next_anim->duration - 0.1)
			return;

		next_anim->assignTime(die);
		final_skeleton = next_anim->skeleton;
		return;
	}
	else {
		next_anim->assignTime(t*next_anim->duration*run_factor); //merge animations
	}

	//final skeleton
	blendSkeleton(&current_anim->skeleton, &next_anim->skeleton, speed_factor, &final_skeleton);

	final_skeleton.updateGlobalMatrices(); //update transforms

}

void Enemy::behaviour_tree(Vector3 front, double dt) {
	Vector3 this_pos = transform.getTranslation();//getting position of the enemy
	Vector3 player_pos = Player::instance->transform.getTranslation(); //getting position of the player

	dist_to_player = this_pos.distance(player_pos);//computing distance to the player

	//default behaviour before seeing the player (rotate the enemy randomly in the Y axis)
	if (!walk && life > 0) {

		if (rot_time == 0) {
			r_angle = random();

			if (r_angle < 0.5)
				r_angle = -r_angle; //sometimes rotate to the other direction

		}

		rot_time += dt;
		if (rot_time > 10) {
			person.angle += dt*r_angle*180; //rotate every 10 seconds
			if (rot_time > 20) {
				rot_time = 0; //after 10 seconds put the time counter to 0
			}
		}
	}

	if (life <= 0) {
		//the enemy dies
		anim_mode = DEATH;
		Vector3 delta_attack = delta_attack * dt * 1000;
		person.speed_attack = person.speed_attack + delta_attack;
		person.speed_attack = person.speed_attack - person.speed_attack*0.05;
		die += dt;

		//death sound
		if (die < 0.1) {
			Audio::manager->playSample("data/Definitivo/AudioSamples/enemy_death.mp3", false, true);
		}

		if (Player::instance->num_enemies > 0 && die > 0 && !stop) {
			Player::instance->num_enemies -= 1; //decrease the number of enemies of the player
			stop = true;
		}
		
		//when the player is too far from the enemy, and the enemy is dead, delete enemy
		if (dist_to_player > 1000) {
			Scene::instance->remove_entity(Scene::instance->entities, this);
			//std::cout << "Bye Enemy!!\n\n" << std::endl;
			//everey time the player kill an enemy, he earn some coins
			Player::instance->player_data.coins += 5;
		}

		return;
	}

	Player::instance->attacked = false;

	Vector3 to_target = this_pos - player_pos; //vector to the player from the enemy

	if (dist_to_player <= 11) {
		if (Player::instance->attack == true) {
			//if the enemy has not seen the player (because is in stealth mode and behind the enemy)
			if (walk == false) {
				//kill if life of enemy is max life, or rest this amount of life (some enemies will have more life)
				life -= MAX_LIFE;
				//audio effect of stealth attack
				Audio::manager->playSample("data/Definitivo/AudioSamples/stealth_attack.mp3", false, true);
				walk = true; //in case that the enemy is alive after this, detect the player
				return;
			}
			//player attacking (random spaced attacks)
			float r_attack = random(20);
			if (r_attack < 0.5 && life > 0) {
				Player::instance->periodic_audio("data/Definitivo/AudioSamples/swordsclash.mp3",
					Player::instance->person.speed_attack.length(),10);
				life -= Player::instance->player_data.damage;
				//std::cout <<"Enemy_life: " << life << std::endl;
			}
		}
	}
	
	//rest the num of enemies of the player by one if the enemy cannot reach the player
	if (dist_to_player > 200) {
		if (walk) {
			if (stop) //in order to do it only once
				return;
			Player::instance->num_enemies -= 1;
			walk = false;
		}
	}

	//if the enemy can see the player, then walk (once the enemy see the player, always will see it) the player can only have 2 enemies at the same time
	if (dist_to_player <= 200 && walk == false && can_see(front, to_target) && Player::instance->num_enemies < 2) {
		//std::cout << "Lets fight!\n\n" << std::endl;
		Audio::manager->playSample("data/Definitivo/AudioSamples/alert_sound.mp3", false, true); //alert the player he has an enemy
		Player::instance->num_enemies += 1;
		walk = true;	
	}

	if (dist_to_player > 200) {
		person.speed = Vector3(); //stop if player is far away
	}

	//if the player is near enough
	if (walk == true) {

		//go to the player
		if (dist_to_player > 10 && Player::instance->player_data.life > 0)
			go_to_target(to_target, front, dt);
		
		rotate_person(to_target, front, dt); //rotate person to the player

		//attack (randomly spaced attacks of the enemy)
		if (dist_to_player <= 11 && Player::instance->player_data.life > 0) {
			float r_attack = random(10);
			if (r_attack < 0.5) {
				//attack sound effect
				Player::instance->periodic_audio("data/Definitivo/AudioSamples/swordsclash2.mp3",
					person.speed_attack.length(), 5);
				attack_player(front,dt);
			}
			return;
		}
		else {
			//if player dead, stop attacking
			anim_mode = FRONT;
		}
	}

}

void Enemy::update(double dt) {

	rot.setRotation(person.angle*DEG2RAD, Vector3(0, 1, 0));
	Vector3 front = rot.rotateVector(Vector3(0, 0, -1));

	
	behaviour_tree(front, dt);
	
	transform.setTranslation(person.pos.x, person.pos.y, person.pos.z);
	transform.rotate(person.angle*DEG2RAD, Vector3(0, 1, 0));

}