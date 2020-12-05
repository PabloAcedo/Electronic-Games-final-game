#ifndef ENEMY_H
#define ENEMY_H

#include "Person.h"
#include "framework.h"

class Enemy : public Person{
public:

	bool walk;
	float life;
	float damage;

	Enemy();
	~Enemy();

	void render_extras(Camera* camera, Light* light);
	void move_skeleton();
	bool can_see(Vector3 front, Vector3 to_target);
	void move_enemy(Vector3 front, double dt, float dir);
	void go_to_target(Vector3 to_target, Vector3 front, double dt);
	void attack_player(Vector3 front, float dt);
	void behaviour_tree(Vector3 front, double dt);
	void update(double dt);
};
#endif
