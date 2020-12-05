#ifndef TARGET_PERSON_H
#define TARGET_PERSON_H

#include "Enemy.h"
//#include "entitymesh.h"

class Target_Person : public Enemy{
public:

	static Target_Person* instance;
	bool active;

	Target_Person();
	Target_Person(float _damage, float _life, const char* texture_name);
	~Target_Person();

	void setFeatures(float _damage, float _life, const char* texture_name);
	void setCoordinates(Vector3 position);
};
#endif
