#ifndef BUYSTAGE_H
#define BUYSTAGE_H

#include "Stage.h"

class Buystage : public Stage
{
public:
	Buystage();
	~Buystage();

	virtual void render();
	virtual void update(double seconds_elapsed);

};
#endif
