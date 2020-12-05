#ifndef ENTITYMESH_H
#define ENTITYMESH_H

#include "entity.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "framework.h"
#include "light.h"



enum eEntitymesh_type : int { NORMAL, INSTANCED, PERSON, MONEY, LIFE };

class Entitymesh : public Entity
{
public:
	Entitymesh();
	~Entitymesh();
	Entitymesh(const char* mesh, const char* texture);

	Mesh* mesh;
	Texture* texture;
	Shader* shader;
	Vector4 color;
	eEntitymesh_type meshtype;
	bool light_trick;

	void render_basic_shader(Camera* camera, Matrix44 model, Light* light);
	virtual void render();
	virtual void update(double dt);

};
#endif
