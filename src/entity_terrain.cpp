#include "entity_terrain.h"
#include "entitymesh.h"
#include "mesh.h"
#include "texture.h"
#include "scene.h"

Texture* texture2 = NULL;
Texture* texture3 = NULL;
Texture* texture_map = NULL;

Entity_terrain::Entity_terrain() : Entitymesh(){


}

Entity_terrain::Entity_terrain(int size) : Entitymesh() {

	Mesh* plane = new Mesh();
	int size_map_coord = size*10; //(every cell of the map is 10 units)
	plane->createPlane(size_map_coord);
	mesh = plane;
	texture = Texture::Get("data/world_elements/floor/grass.tga");
	texture2 = Texture::Get("data/world_elements/floor/pavement_color.tga");
	texture3 = Texture::Get("data/world_elements/floor/sand.tga");
	texture_map = Texture::Get("data/world_elements/floor/texture_map.tga");
	transform.setTranslation(size_map_coord, 0, size_map_coord);
	texture_tiling = 256.0f;
	shader = Shader::Get("data/shaders/basic_light.vs", "data/shaders/texture_light_terrain.fs");

}


Entity_terrain::~Entity_terrain(){

}

void Entity_terrain::render() {
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
	shader->setTexture("u_texture2", texture2, 1);
	shader->setTexture("u_texture3", texture3, 2);
	shader->setTexture("u_texture_map", texture_map, 3);
	render_basic_shader(camera, model, light);

	//render the mesh using the shader
	mesh->render(GL_TRIANGLES, -1);

	//disable the shader after finishing rendering
	shader->disable();
}
