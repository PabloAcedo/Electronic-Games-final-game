#ifndef LEVELCREATOR_H
#define LEVELCREATOR_H

#include "levelmap.h"
//#include "entitymesh.h"


class LevelCreator{

public:

	levelmap* level;

	LevelCreator();
	LevelCreator(const char* level_map_name);
	//LevelCreator(float _target_damage, float _target_life, const char* target_texture_name);

	~LevelCreator();

	//in order to change the target_person features and appearance
	//void setTarget_things(float _target_damage, float _target_life, const char* target_texture_name);
};

#endif
