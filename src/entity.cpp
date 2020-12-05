#include "entity.h"
#include "includes.h"
#include "utils.h"
#include "framework.h"
#include "scene.h"


Entity::Entity(){
	parent = NULL;
	show = true;
}


Entity::~Entity(){
	
}


void Entity::addChild(Entity* &ent) {
	ent->parent = this; //inserting the child parent
	Scene::instance->add_entity(children, ent);
}

void Entity::removeChild(Entity* &ent) {

	Scene::instance->remove_entity(children, ent);//erasing the entity from the children vector
	ent->~Entity(); //removing the entity (deallocate memory reserved for the entity)
	
}

Matrix44 Entity::getGlobalMatrix() {
	if (parent) //if I have a parent, ask his global and concatenate
		return transform * parent->getGlobalMatrix();
	return transform; //otherwise just return my model as global
}

void Entity::render(){

}

void Entity::update(double seconds_elapsed) {


}
