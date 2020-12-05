#ifndef ENTITY_TERRAIN_H
#define ENTITY_TERRAIN_H


#include "entitymesh.h"


class Entity_terrain : public Entitymesh
{
public:
	
	Entity_terrain();
	Entity_terrain(int size);
	~Entity_terrain();

	void render();
};
#endif
