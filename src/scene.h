#ifndef SCENE_H
#define SCENE_H

#include "entity.h"
#include "entitymesh.h"
#include "levelmap.h"
#include "light.h"
#include "framework.h"
#include <map>

enum eLeveltype : uint8  {EXISTS, NOEXISTS};

class Scene
{
public:

	static Scene* instance;

	Vector3 skycolor;

	std::vector<Entity*> entities;

	std::vector<Entity*> temporal_entities;//in order to remove the temporal entities of different levels

	levelmap* current_level;

	levelmap* next_level;
	
	const char* levelName;

	eLeveltype exists_level;

	Light* main_light;

	float far_plane;

	bool gamePad;

	//maps manager container
	std::map<std::string, levelmap*> level_maps;

	Scene();
	~Scene();

	//methods
	void add_entity(std::vector<Entity*>& entities, Entity* ent);
	std::pair<bool, int > findInVector(std::vector<Entity*> entities, Entity* ent);
	void remove_entity(std::vector<Entity*>& entities, Entity* ent);
	bool inArea(Vector3 current_pos, Vector3 entity_pos, int area);
	std::vector<Entitymesh*> near_ent(Matrix44 transform, int area);
	Vector2 world2Cell(float x, float z);
	void Init();

	void mesh2Entity(Entitymesh* &ent);
	void change_level(int level);
	void clear_level();
	void begin_level(int level);

	void renderGUI(int centerx, int centery, int w, int h, const char* texture_name, Vector4 tex_range);
	void renderButton(int centerx, int centery, int w, int h, const char* texture_name, Vector4 tex_range, bool inOption);

	void render(float view);
	void update(double seconds_elapsed);
};
#endif

