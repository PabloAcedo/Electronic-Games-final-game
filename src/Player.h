#ifndef PLAYER_H
#define PLAYER_H

#include "Person.h"
#include "camera.h"
#include "light.h"
#include "entity.h"
#include "Mission_Person.h"

#define COIN_VAL 10
#define MAX_LIFE 100
#define FOOD_EXTRA 10


//main properties of the player (compacted in struct in order to save this data more efficiently)
struct sPlayer {
	int coins;
	float damage;
	float life;
	sPlayer() { coins = 0; damage = 5;  life = 100;}
};

class Player : public Person
{
public:

	bool attacked; bool attack; bool stealth; bool vista; bool enter_mission; bool mission_mode;

	bool fbo_active;

	Shader* fx;

	int num_enemies;

	e_Animation anim_mode;

	static Player* instance;
	sPlayer player_data;

	Entity* target_entity;

	Player();
	~Player();

	void periodic_audio(const char* filename, float speed, int period);
	void restartLife();
	void behaviourtree(double dt);
	void changeTargetentity(eMissionState mission_state);
	void collect();
	void change_animation(e_Animation mode);
	void render_extras(Camera* camera, Light* light);
	void move_skeleton();
	void move_player(double dt, float dir);
	void update(double dt);
};
#endif
