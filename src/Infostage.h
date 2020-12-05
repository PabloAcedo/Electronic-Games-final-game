#ifndef INFOSTAGE_H
#define INFOSTAGE_H

#include "Stage.h"

class Infostage : public Stage
{
public:
	Infostage();
	~Infostage();

	virtual void render();
	virtual void update(double seconds_elapsed);

};
#endif
