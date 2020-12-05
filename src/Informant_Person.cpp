#include "Informant_Person.h"
#include "Player.h"



Informant_Person::Informant_Person() : Person("data/Definitivo/main_character/ninja.mesh", "data/Definitivo/Persons_and_enemies/japan_peasant.tga") {
	ID = 0;
	name = "informant";
	active = false;
}

Informant_Person::Informant_Person(int _ID) : Person("data/Definitivo/main_character/ninja.mesh", "data/Definitivo/Persons_and_enemies/japan_peasant.tga") {
	ID = _ID;
	name = "informant";
	active = false;
}


Informant_Person::~Informant_Person()
{
}

void Informant_Person::update(double dt) {

	rot.setRotation(person.angle*DEG2RAD, Vector3(0, 1, 0));
	Vector3 front = rot.rotateVector(Vector3(0, 0, -1));

	Vector3 this_pos = transform.getTranslation();//getting position of the informant
	Vector3 player_pos = Player::instance->transform.getTranslation(); //getting position of the player

	dist_to_player = this_pos.distance(player_pos);//computing distance to the player

	Vector3 to_target = this_pos - player_pos;

	rotate_person(to_target, front, dt);

	transform.setTranslation(person.pos.x, person.pos.y, person.pos.z);
	transform.rotate(person.angle*DEG2RAD, Vector3(0, 1, 0));


}
