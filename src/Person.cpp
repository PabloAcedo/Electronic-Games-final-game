#include "Person.h"
#include "entity.h"
#include "entitymesh.h"
#include "input.h"
#include "scene.h"
#include "levelmap.h"
#include "animation.h"
#include "shader.h"
#include "game.h"
#include "camera.h"
#include "light.h"



Entitymesh* ent = NULL;

//default
Person::Person() : Entitymesh("data/Definitivo/main_character/ninja.obj", "data/Definitivo/Persons_and_enemies/japan_peasant.tga"){
	meshtype = PERSON;
	name = "person";
	anime = true;
	die = 0;
}

Person::Person(const char* mesh_name, const char* texture_name) : Entitymesh() {
	texture = Texture::Get(texture_name);
	mesh = Mesh::Get(mesh_name);
	meshtype = PERSON;
	name = "person";

	current_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_idle2.skanim");
	next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_running_enemy.skanim");

	shader = Shader::Get("data/shaders/skinning_light.vs", "data/shaders/texture_light.fs");
	anime = true;
	die = 0;
}



Person::~Person(){
	
}


void Person::collision(Vector3& target, Vector3& coll, Vector3& norm, std::vector<Entitymesh*> near_entities, float dt) {

	Vector3 center = target + Vector3(0, 6, 0);

	for (int i = 0; i < near_entities.size(); i++) {

		//do not collide with itself
		if (near_entities[i] == this) {
			continue;
		}

		ent = near_entities[i];

		//testing collision
		if (ent->mesh->testSphereCollision(ent->transform, center, 2, coll, norm) == false)
			continue;

		//moving the position if there is colision
		Vector3 push = normalize(coll - center)*dt*5;
		col = true;
		target = person.pos - push;
		target.y = 0;
		break;
	}

}

void Person::change_animation(e_Animation mode) {
	switch (mode) {
	case FRONT:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_running_enemy.skanim");
		break;
	case BACK:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_running_backward.skanim");
		break;
	case ATTACK:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/enemy_attack.skanim");
		break;
	case DEATH:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_death.skanim");
		break;
	case HELLO:
		next_anim = Animation::Get("data/Definitivo/main_character/Animations/animations_waving.skanim");
		break;
	}
}


void Person::move_skeleton() {

	float speed_factor = person.speed.length()*0.1;

	float run_factor = 1;
	
	current_anim->assignTime(Game::instance->time);
	float t = fmod(Game::instance->time, current_anim->duration) / current_anim->duration;

	if (speed_factor >= 30) {
		run_factor *= 2;
	}

	next_anim->assignTime(t*next_anim->duration*run_factor * 20);

	blendSkeleton(&current_anim->skeleton, &next_anim->skeleton, speed_factor,&final_skeleton);

}

void Person::rotate_person(Vector3 to_target, Vector3 front, double dt) {
	to_target.normalize(); front.normalize();
	float to_target_angle = atan2(to_target.x, to_target.z);
	float front_angle = atan2(front.x, front.z);
	float rot_angle = (front_angle - to_target_angle) * 180 / PI;
	person.angle += rot_angle * 8 * dt;
}

void Person::render_extras(Camera* camera, Light* light) {

}

void Person::render() {
	if (!shader)
		return;

	//get the last camera that was activated
	Camera* camera = Camera::current;
	Light* light = Scene::instance->main_light;//getting the light
	Matrix44 model = getGlobalMatrix();

	//testing if the camera sees the object
	BoundingBox global_box = transformBoundingBox(model, mesh->box);
	if (!camera->testBoxInFrustum(global_box.center, global_box.halfsize))
		return;

	//enable shader and pass uniforms
	shader->enable();

	move_skeleton();

	render_basic_shader(camera, model, light);

	
	mesh->renderAnimated(GL_TRIANGLES, &final_skeleton);
	
	
	//disable the shader after finishing rendering
	shader->disable();

	for (int i = 0; i < children.size(); i++)
		if (children[i]->show)
			children[i]->render();  //repeat for every child

	render_extras(camera, light);
}

void Person::update(double dt){

	transform.setTranslation(person.pos.x, person.pos.y, person.pos.z);
	transform.rotate(person.angle*DEG2RAD, Vector3(0, 1, 0));
	
}