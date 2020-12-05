
#ifndef PLAYSTAGE_H
#define PLAYSTAGE_H


#include "Stage.h"


class Playstage : public Stage
{
public:

	Playstage();
	~Playstage();

	void render();
	void update(double seconds_elapsed);

};
#endif