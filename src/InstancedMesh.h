#ifndef INSTANCEDMESH_H
#define INSTANCEDMESH_H

#include "entitymesh.h"
#include "texture.h"

//enum eInstancedType : uint8 { FOREST, NOFOREST };

class InstancedMesh : public Entitymesh
{
public:

	std::vector<Matrix44> transforms; //vector of transforms
	Image* density_img;
	//bool render_this;
	//eInstancedType type;

	InstancedMesh();
	InstancedMesh(const char* mesh_name, const char* texture_name, const char* density_img);
	InstancedMesh(const char* mesh_name, const char* texture_name);
	~InstancedMesh();

	//de prueba, borrar
	void random_positions();
	virtual void render();
	virtual void update(float dt);
};
#endif
