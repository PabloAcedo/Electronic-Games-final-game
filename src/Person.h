#ifndef PERSON_H
#define PERSON_H

#include "entitymesh.h"
#include "framework.h"
#include "scene.h"
#include "animation.h"

enum ePersonType : uint8 {
	NOTHING, PLAYER, PEASANT, SOLDIER
};

enum e_Animation { IDLE, FRONT, BACK, ATTACK, DEATH, STEALTH, HELLO };

struct sPerson {
	Vector3 pos;
	Vector3 speed;
	Vector3 speed_attack;
	float angle;
	float angle_speed;
	sPerson() { angle = angle_speed = 0.0; }
};

class Person: public Entitymesh
{
public:
	ePersonType pers_type; 

	Animation* current_anim; Animation* next_anim;

	e_Animation anim_mode;

	int speed;
	sPerson person;//person data 
	bool col; //collision state
	Matrix44 rot;//rotation matrix
	bool is_target;

	float dist_to_player;

	Skeleton final_skeleton;
	bool anime;

	std::vector<Entitymesh*> near_entities; //entities that are near to the person

	float die;
	float stop;

	//methods
	Person();
	Person(const char* mesh_name, const char* texture_name);
	~Person();

	virtual void move_skeleton();
	void rotate_person(Vector3 to_target, Vector3 front, double dt);
	virtual void change_animation(e_Animation mode);
	virtual void render_extras(Camera* camera, Light* light);
	virtual void render();
	virtual void collision(Vector3& target, Vector3& coll, Vector3& norm, std::vector<Entitymesh*> near_entities, float dt);
	virtual void update(double dt);

};
#endif
