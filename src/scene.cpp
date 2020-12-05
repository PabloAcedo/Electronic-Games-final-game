#include "scene.h"
#include "entity.h"
#include "camera.h"
#include "input.h"
#include "game.h"
#include "scene.h"
#include "entity.h"
#include "entitymesh.h"
#include "camera.h"
#include "Person.h"
#include "entity_terrain.h"
#include "Player.h"
#include "InstancedMesh.h"
#include "Mission_Person.h"
#include "Target_Person.h"
#include "texture.h"
#include "mesh.h"
#include "shader.h"
#include "Audio.h"

#define STANDARD_LIFE 100.0f

#define STANDARD_DAMAGE 2.0f

Camera* cam = NULL; //main camera
Player* Player::instance = NULL; //player
Mission_Person* Mission_Person::instance = NULL;//mission person
Target_Person* Target_Person::instance = NULL;//target person
Audio* Audio::manager = NULL;//Audio manager in the game



Scene::Scene()
{
	instance = this;
	Init();
}


Scene::~Scene(){

}

void Scene::Init() {

	skycolor = Vector3(0.1,0.1,0.1);//color of the sky
	//creating the map (contains info about position of entities and adds to the entities array the static ones)
	current_level = new levelmap(256, 256, "data/maps/prueba.map",false); //this is the main map

	next_level = new levelmap(256, 256); //init the levels

	//main light of the world 
	main_light = new Light();
	Vector3 light_start_pos = Scene::instance->current_level->set_start_position(SUN);
	main_light->position = Vector3(light_start_pos.x, 1000, light_start_pos.z);
	
	//terrain of the map
	Entitymesh* suelo = new Entity_terrain(current_level->w);
	Scene::instance->add_entity(entities, suelo);

	//player
	Player::instance = new Player();
	Scene::instance->add_entity(entities, Player::instance);

	//mission person
	Mission_Person::instance = new Mission_Person();
	Scene::instance->add_entity(entities, Mission_Person::instance);

	//target person
	Target_Person::instance = new Target_Person();

	//init audio
	Audio::manager = new Audio();

	//get this sound at the beggining
	Audio::manager->Get("data/Definitivo/AudioSamples/Wind_backgroundSound.mp3");

	//creating the camera
	far_plane = 1500.f;
	cam = new Camera();
	cam->lookAt(Vector3(0.f, 10.f, 10.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	cam->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, far_plane); //set the projection, we want to be perspective

}

void Scene::mesh2Entity(Entitymesh* &ent) {
	(Entity*)ent;
}

void Scene::add_entity(std::vector<Entity*>& entities, Entity* ent) {
	entities.push_back(ent);
}

std::pair<bool, int > Scene::findInVector(std::vector<Entity*> entities, Entity* ent) {
	std::pair<bool, int > index;

	// Find given element in vector
	auto it = std::find(entities.begin(), entities.end(), ent);

	if (it != entities.end()) //if the element is found
	{
		index.second = std::distance(entities.begin(), it); //return the index
		index.first = true;
	}
	else
	{
		index.first = false; //element not found
		index.second = -1;
	}

	return index;
}

void Scene::remove_entity(std::vector<Entity*>& entities, Entity* ent) {
	std::pair<bool, int> index = findInVector(entities, ent); //finding in children vector the entity

	if (index.first) {
		int i = index.second;
		entities.erase(entities.begin() + i); //erasing the entity
	}
	else {
		std::cout << "Entity not found, cannot remove it." << std::endl;
		return;
	}
}

//go from wolrd space to cell (map) space
Vector2 Scene::world2Cell(float x, float z) {

	Vector2 cell_position;

	cell_position.x = floor(x/current_level->dist);
	cell_position.y = floor(z / current_level->dist);

	return cell_position;
}

//method in order to know if something (an entity) is near (another one) in a given area, given its positions 
bool Scene::inArea(Vector3 current_pos, Vector3 entity_pos, int area) {

	float distance = current_pos.distance(entity_pos);

	if (distance < area) {
		return true;
	}

	return false;
}

//method in order to know which entities are near another one
std::vector<Entitymesh*> Scene::near_ent(Matrix44 transform, int area) {

	std::vector<Entitymesh*> near_ent2;//array to return

	Vector3 current_pos = transform.getTranslation(); //position of the given entity

	for (int i = 0; i < entities.size(); i++) {
		

		if (entities[i]->type != MESH)
			continue;

		Entitymesh* ent = (Entitymesh*)entities[i];//downcasting to entitymesh in order to access to the mesh

		/*including big entities always (because the translation of the transform 
		do not cover the position of all the mesh because the size of it)*/
		if (ent->name == "wall") {
			near_ent2.push_back(ent); 
			continue;
		}
		else if (ent->name == "mountain") {
			continue;
		}

		if (ent->meshtype == INSTANCED) { //if the entity uses instancing
			InstancedMesh* ient = (InstancedMesh*)ent; //downcasting to instancedMesh in order to access to the models array
			for (int j = 0; j < ient->transforms.size(); j++) { //iterate over all the models of the entity
				Matrix44 m = ient->transforms[j]; //problems sometimes (low posibility of problems)
				Vector3 entity_pos = m.getTranslation(); //getting the position
				if (inArea(current_pos, entity_pos, area)) { //if is near the player
					ient->transform = m;
					near_ent2.push_back(ient); //add this entity to the near list
				}
			}
		}
		else {
			//entities without instancing
			Vector3 entity_pos = ent->transform.getTranslation();//position of each entity in the world
			if (inArea(current_pos, entity_pos, area)) {
				near_ent2.push_back(ent);
			}
		}
	}
	//std::cout <<near_ent2.size()<< std::endl;
	return near_ent2;
}

void Scene::render(float view) {

	//placing the camera attached to the player
	Vector3 eye;
	Vector3 center;
	Vector3 up;

	//normal view
	if (!Player::instance->vista) {
		eye = Player::instance->transform * Vector3(0, 21, -32);
		center = Player::instance->transform * Vector3(0, 7.8, 0);
		up = Player::instance->transform.rotateVector(Vector3(0, 1, 0));
	}
	else {
		//event view (talking, etc)
		eye = Player::instance->transform * Vector3(20, view, -20);
		center = Player::instance->transform * Vector3(0, 7.8, view);
		up = Player::instance->transform.rotateVector(Vector3(0, 1, 0));
	}

	cam->enable();
	if (!Game::instance->free_cam)
		cam->lookAt(eye, center, up);

	Scene::instance->main_light->position = eye-Vector3(0,0,10); //position of the light

	//rendering every entity
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->render();
	}
}

void Scene::clear_level() {
	//clear all the entities of the level except the target
	while (temporal_entities.size() != 0) {
		for (int i = 0; i < temporal_entities.size(); i++) {
			Entity* ent = temporal_entities[i];
			remove_entity(entities, ent);
			remove_entity(temporal_entities, ent);
		}
	}
}

void Scene::change_level(int level) {

	const char* levelNames[] = { {"data/maps/level0.map"},{"data/maps/level1.map"} };
	const char* targetTexturesN[] = { {"data/Definitivo/Persons_and_enemies/target1.tga"},
									  {"data/Definitivo/Persons_and_enemies/target2.tga"} };

	int maxLev = 1;

	//increasing the difficulty automatically
	int difficulty = level + 1;
	float target_damage = STANDARD_DAMAGE * float(difficulty);  float target_life = STANDARD_LIFE * float(difficulty);
	
	//charging levels if level exists
	if (level >= 0 && level <= maxLev) {
		//new level
		levelName = levelNames[level];
		//reset target person
		Target_Person::instance->setFeatures(target_damage, target_life, targetTexturesN[level]);
		exists_level = EXISTS;
	}
	else {
		exists_level = NOEXISTS;
		return;
	}
	
	//init target person features
	Target_Person::instance->walk = false;
	Target_Person::instance->anim_mode = FRONT;
	Target_Person::instance->die = 0;

}

void Scene::begin_level(int level) {

	change_level(level);//changing level name

	//if no more levels, do not create more levels
	if (exists_level == NOEXISTS)
		return;

	//creating the level
	next_level = levelmap::Get(levelName);
	next_level->createLevel();

	//adding the target
	Vector3 position = next_level->set_start_position(TARGET);
	Target_Person::instance->setCoordinates(position);
}

void Scene::renderGUI(int centerx, int centery, int w, int h, const char* texture_name, Vector4 tex_range) {
	
	//GUI renderer

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	Mesh quad;
	quad.createQuad(centerx,centery,w,h,false);

	Camera cam2D;
	cam2D.setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1, 1);
	cam2D.enable();

	Matrix44 m;

	Texture* tex = Texture::Get(texture_name);

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/gui.fs");
	shader->enable();
	shader->setUniform("u_color", Vector4(1,1,1,1));
	shader->setUniform("u_model", m);
	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
	shader->setTexture("u_texture", tex, 0);
	shader->setUniform("u_tex_range", tex_range);
	quad.render(GL_TRIANGLES);
	shader->disable();

}

void Scene::renderButton(int centerx, int centery, int w, int h, const char* texture_name, Vector4 tex_range, bool inOption) {
	//buttons renderer

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	Mesh quad;
	quad.createQuad(centerx, centery, w, h, false);

	Camera cam2D;
	cam2D.setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1, 1);
	cam2D.enable();

	Matrix44 m;

	Texture* tex = Texture::Get(texture_name);

	Vector4 color_hover = Vector4(1, 1, 1, 0.6);
	Vector4 normal_color = Vector4(1, 1, 1, 1);

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/gui.fs");
	shader->enable();
	shader->setUniform("u_color", inOption ? normal_color : color_hover);
	shader->setUniform("u_model", m);
	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
	shader->setTexture("u_texture", tex, 0);
	shader->setUniform("u_tex_range", tex_range);
	quad.render(GL_TRIANGLES);
	shader->disable();
}

void Scene::update(double seconds_elapsed) {

	float speed = seconds_elapsed * 100;
	//mouse to rotate the camera
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
	{
		cam->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		cam->rotate(Input::mouse_delta.y * 0.005f, cam->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	if (Game::instance->free_cam == false)
	{
		//updating all entities (right now only updating the player)
		for (int i = 0; i < entities.size(); i++) {
			entities[i]->update(seconds_elapsed);
		}
	}
	else //mode free cam
	{
		//async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) cam->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) cam->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) cam->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) cam->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	}

	if (Input::isKeyPressed(SDL_SCANCODE_4)) {
		std::cout << "eye: " << cam->eye.x << ", " << cam->eye.y << ", " << cam->eye.z << std::endl;
		std::cout << "center: " << cam->center.x << ", " << cam->center.y << ", " << cam->center.z << std::endl;
		std::cout << "up: " << cam->up.x << ", " << cam->up.y << ", " << cam->up.z << std::endl;
	}
	

}
