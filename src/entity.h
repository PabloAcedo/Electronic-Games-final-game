#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"

enum eEntity_type : uint8 {
	MESH
};

class Entity
{
public:
	std::string name;
	Matrix44 transform;

	//pointer to my parent entity 
	Entity* parent;
	//pointers to my children
	std::vector<Entity*> children;
	eEntity_type type;
	float texture_tiling;
	bool show;

	Entity();
	~Entity();

	virtual void render();
	virtual void update(double elapsed_time);

	//methods
	void addChild(Entity* &ent);
	void removeChild(Entity* &ent);

	Matrix44 getGlobalMatrix(); //returns transform in world coordinates

	//some useful methods...
	//Vector3 getPosition();

};

#endif
