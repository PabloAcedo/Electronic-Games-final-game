#ifndef TUTORIALSTAGE_H
#define TUTORIALSTAGE_H

#include "Stage.h"

class Tutorialstage : public Stage{
public:
	Tutorialstage();
	~Tutorialstage();


	void render();
	void update(double dt);
};
#endif
