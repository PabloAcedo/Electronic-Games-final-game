#include "levelmap.h"
#include "utils.h"
#include "includes.h"
#include "mesh.h"
#include "texture.h"
#include "framework.h"
#include "camera.h"
#include "shader.h"
#include "entity.h"
#include "entitymesh.h"
#include "scene.h"
#include "InstancedMesh.h"
#include "Person.h"
#include "Enemy.h"
#include "Mission_Person.h"
#include "Informant_Person.h"
#include "light.h"

#include <cmath>
#include <cassert>
#include <map>

#define MAX_LIGHTS 4


levelmap::levelmap(int w, int h, const char* level_name, bool temporal)
{
	data = new sCell[w*h];
	loadMap(level_name);
	dist = 20;

	temporal_entities = temporal;

	if (!temporal)
		createMap();

}

levelmap::levelmap(int w, int h) {
	data = new sCell[w*h];
	dist = 20;
	temporal_entities = true;
}



levelmap::levelmap()
{
	
}

void levelmap::Init(int nc, int cs){

}

sCell& levelmap::getCell(int x, int y)
{
	return data[x + y * header.w];
}

void levelmap::setCell(int x, int y, eCellType type) {
	data[x + y * header.w].type = type;
}

bool levelmap::loadMap(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL) //file not found
		return false;

	
	fread(&header, sizeof(sMapHeader), 1, file);

	assert(header.bytes == 1); //always control bad cases!!

	//allocate memory for the cells data and read it
	unsigned char* cells = new unsigned char[header.w*header.h];
	fread(cells, header.bytes, header.w*header.h, file);

	w = header.w;
	h = header.h;
	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
			getCell(x, y).type = (eCellType)cells[x + y * w];

	delete[] cells; //always free any memory allocated!

	set_items();

	return true;
}

levelmap* levelmap::Get(const char* filename) {
	assert(filename);

	//check if loaded
	auto it = Scene::instance->level_maps.find(filename);
	if (it != Scene::instance->level_maps.end()) {
		std::cout << "Currently uploaded: " << filename << std::endl;
		return it->second;
	}
		

	//load it
	levelmap* lev = new levelmap(256,256);
	if (!lev->loadMap(filename))
	{
		delete lev;
		return NULL;
	}

	Scene::instance->level_maps[filename] = lev;
	std::cout << "Uploaded map: " << filename << std::endl;
	
	return lev;

}


void levelmap::createMap() {

	//creating forest (using instancing and placing each tree according to a density map)
	InstancedMesh* forest = new InstancedMesh("data/Definitivo/trees/tall_pine.obj", "data/Definitivo/trees/tall_pine.png", "data/world_elements/floor/density_map.png");
	forest->light_trick = true;
	Scene::instance->add_entity(Scene::instance->entities, forest);

	//entities using instancing
	InstancedMesh* sakura = new InstancedMesh("data/Definitivo/trees/sakura.obj", "data/Definitivo/trees/sakura.png");
	InstancedMesh* city_house = new InstancedMesh("data/Definitivo/Houses/house_j_buena.obj", "data/Definitivo/Houses/Textures/japan_house.tga");
	InstancedMesh* old_house = new InstancedMesh("data/Definitivo/Houses/house_peasant.obj", "data/Definitivo/Houses/Textures/house_peasant.png");
	InstancedMesh* h_mountain = new InstancedMesh("data/Definitivo/mountains/mountain_horizontal.obj", "data/Definitivo/mountains/corner1.png");
	h_mountain->name = "mountain";
	InstancedMesh* v_mountain = new InstancedMesh("data/Definitivo/mountains/mountain_vertical.obj", "data/Definitivo/mountains/corner1.png");
	v_mountain->name = "mountain";

	//model matrix for entities using instancing (except the forest)
	Matrix44 m;
	int light_num = 0;
	for (int x = 0; x < w; ++x){

		for (int z = 0; z < h; ++z){

			//get cell info
			sCell& cell = getCell(x, z);
			if (cell.printable == false) {
				continue;
			}

			eCellType type = cell.type;
			Entitymesh* ent = NULL;
			switch (type) {
			case HOUSE:
				m.setTranslation(x*dist, 0, z*dist);
				city_house->transforms.push_back(m);
				city_house->name = "house";
				break;
			case COIN:
				ent = new Entitymesh("data/Definitivo/Items/coin_bag2.obj", "data/Definitivo/Items/coin_bag2.png");
				ent->name = "coin";
				ent->meshtype = MONEY;
				break;
			case ENEMY:
				ent = new Enemy();
				break;
			case WALL:
				ent = new Entitymesh("data/Definitivo/Houses/wall_horizontal.obj", "data/Definitivo/Houses/Textures/muro.tga");
				ent->name = "wall";
				break;
			case WALL_VERT:
				ent = new Entitymesh("data/Definitivo/Houses/muro_vert.obj", "data/Definitivo/Houses/Textures/muro.tga");
				ent->name = "wall";
				break;
			case WALL_ENTRANCE:
				ent = new Entitymesh("data/Definitivo/Houses/wall_entrance.obj", "data/Definitivo/Houses/Textures/wall_entrada_buena.tga");
				ent->name = "wall";
				break;
			case PINE:
				ent = new Entitymesh("data/Definitivo/trees/tall_pine.obj", "data/Definitivo/trees/tall_pine.png");
				ent->name = "pine";
				ent->light_trick = true;
				break;
			case SAKURA:
				m.setTranslation(x*dist, 0, z*dist);
				m.rotate(random() * 180 * DEG2RAD, Vector3(0, 1, 0));
				sakura->transforms.push_back(m);
				sakura->name = "sakura";
				sakura->light_trick = true;
				break;
			case HOUSE_OLD:
				m.setTranslation(x*dist, 0, z*dist);
				m.rotate(random() * 180 * DEG2RAD, Vector3(0, 1, 0));
				old_house->transforms.push_back(m);
				old_house->name = "old_house";
				break;
			case FOOD:
				ent = new Entitymesh("data/Definitivo/Items/cherries.obj", "data/Definitivo/Items/cherries.png");
				ent->name = "food";
				ent->meshtype = LIFE;
				break;
			case MOUNTAINH:
				m.setTranslation(x*dist, 0, z*dist);
				h_mountain->transforms.push_back(m);
				break;
			case MOUNTAINV:
				m.setTranslation(x*dist, 0, z*dist);
				v_mountain->transforms.push_back(m);
				break;
			case CORNER1:
				ent = new Entitymesh("data/Definitivo/mountains/esquina_mountains_norte1.obj", "data/Definitivo/mountains/corner1.png");
				ent->name = "mountain";
				ent->texture_tiling =100.0;
				break;
			case CORNER2:
				ent = new Entitymesh("data/Definitivo/mountains/esquina_mountains_norte2.obj", "data/Definitivo/mountains/corner1.png");
				ent->texture_tiling = 100.0;
				ent->name = "mountain";
				break;
			case CORNER3:
				ent = new Entitymesh("data/Definitivo/mountains/esquina_mountains_sur1.obj", "data/Definitivo/mountains/corner1.png");
				ent->name = "mountain";
				ent->texture_tiling = 100.0;
				break;
			case CORNER4:
				ent = new Entitymesh("data/Definitivo/mountains/esquina_mountains_sur2.obj", "data/Definitivo/mountains/corner1.png");
				ent->name = "mountain";
				ent->texture_tiling = 100.0;
				break;
			}

			if (ent == NULL)
				continue;

			Vector3 position = Vector3(x*dist, 0, z*dist);
			ent->transform.setTranslation(position.x, 0, position.z);

			if (ent->meshtype == PERSON) {
				Person* aux = (Person*)ent;
				aux->person.pos = position;
				ent = (Entitymesh*)aux;
			}

			Scene::instance->add_entity(Scene::instance->entities, ent);

			if (temporal_entities == true) {
				Scene::instance->add_entity(Scene::instance->temporal_entities, ent);
			}
			

		}
	}

	//add mesh instanced entities
	if (temporal_entities == false) {
		Scene::instance->add_entity(Scene::instance->entities, sakura);
		Scene::instance->add_entity(Scene::instance->entities, city_house);
		Scene::instance->add_entity(Scene::instance->entities, old_house);
		Scene::instance->add_entity(Scene::instance->entities, v_mountain);
		Scene::instance->add_entity(Scene::instance->entities, h_mountain);
	}

}


void levelmap::createLevel() {

	//loadMap(filename);

	Mission_Person::instance->informantNum = 0;

	for (int x = 0; x < w; ++x) {

		for (int z = 0; z < h; ++z) {

			//get cell info
			sCell& cell = getCell(x, z);
			if (cell.printable = false) {
				continue;
			}

			eCellType type = cell.type;
			Entitymesh* ent = NULL;
			switch (type) {
			case ENEMY:
				ent = new Enemy();
				break;
			case INFORMANT:
				Mission_Person::instance->informantNum += 1;
				ent = new Informant_Person(Mission_Person::instance->informantNum);
				break;
			}

			if (ent == NULL)
				continue;

			Vector3 position = Vector3(x*dist, 0, z*dist);
			ent->transform.setTranslation(position.x, 0, position.z);

			if (ent->meshtype == PERSON) {
				Person* aux = (Person*)ent;
				aux->person.pos = position;
				ent = (Entitymesh*)aux;
			}

			Scene::instance->add_entity(Scene::instance->entities, ent);

			if (type == INFORMANT) {
				Mission_Person::instance->informant = (Informant_Person*)ent;
				std::cout << "Informante añadido" << std::endl;
				
			}

			if (temporal_entities == true) {
				Scene::instance->add_entity(Scene::instance->temporal_entities, ent);
			}


		}
	}

}

Vector3 levelmap::set_start_position(eCellType type) {

	Vector3 result;
	for (int x = 0; x < w; ++x)
		for (int z = 0; z < h; ++z)
		{
			sCell cell = getCell(x, z);
			if (cell.type == type) {
				//in order to be in world 2D coordinates (not map)
				result.x = x * dist;
				result.y = 0;
				result.z = z * dist;
				break;
			}
		}

	return result;
}


void levelmap::set_items() {
	for (int x = 0; x < w; x++)
		for (int z = 0; z < h; z++) {
			sCell& cell = getCell(x, z);
			if (cell.type == HOUSE || cell.type == ENEMY || cell.type == WALL
				|| cell.type == WALL_VERT || cell.type == WALL_ENTRANCE 
				|| cell.type == PINE || cell.type == SAKURA || cell.type == COIN
				|| cell.type == HOUSE_OLD || cell.type == FOOD || cell.type == INFORMANT
				|| cell.type == MOUNTAINH || cell.type == MOUNTAINV || cell.type == CORNER1
				|| cell.type == CORNER2 || cell.type == CORNER3 || cell.type == CORNER4) {
				data[x + z * w].printable = true;
			}
			else {
				data[x + z * w].printable = false;
			}
		}
}

levelmap::~levelmap()
{
}


