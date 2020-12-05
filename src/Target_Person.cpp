#include "Target_Person.h"
#include "Enemy.h"
#include "Person.h"
#include "texture.h"


Target_Person::Target_Person() : Enemy(){
	instance = this;
}

Target_Person::Target_Person(float _damage, float _life, const char* texture_name) : Enemy() {
	instance = this;
	setFeatures(_damage, _life, texture_name);
}


Target_Person::~Target_Person()
{
}

void Target_Person::setFeatures(float _damage, float _life, const char* texture_name) {
	damage = _damage;
	life = _life;
	texture = Texture::Get(texture_name);
}

void Target_Person::setCoordinates(Vector3 position) {

	person.pos = position;
	transform.translate(person.pos.x, 0, person.pos.z);

}
