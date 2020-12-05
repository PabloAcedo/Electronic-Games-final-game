#ifndef INFORMANT_PERSON_H
#define INFORMANT_PERSON_H

#include "Person.h"

class Informant_Person : public Person{

public:

	int ID;
	bool active;

	Informant_Person();
	Informant_Person(int _ID);
	~Informant_Person();

	void update(double dt);

};
#endif
