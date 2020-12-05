#ifndef PAUSESTAGE_H
#define PAUSESTAGE_H

#include "Stage.h"
class Pausestage : public Stage
{
public:
	Pausestage();
	~Pausestage();

	virtual void render();
	virtual void update(double seconds_elapsed);
};

#endif
