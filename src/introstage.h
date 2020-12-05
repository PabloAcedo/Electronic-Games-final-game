
#ifndef INTROSTAGE_H
#define INTROSTAGE_H


#include "Stage.h"


class Introstage : public Stage
{
public:

	//constructor
	Introstage(const char* intro, const char* menu_sprite);
	Introstage();

	virtual void render();
	virtual void update(double seconds_elapsed);


};
#endif

