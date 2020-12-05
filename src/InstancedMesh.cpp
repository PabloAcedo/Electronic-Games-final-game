#include "InstancedMesh.h"
#include "scene.h"
#include "entitymesh.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "Player.h"


InstancedMesh::InstancedMesh() : Entitymesh( ){
	meshtype = INSTANCED;
}

InstancedMesh::InstancedMesh(const char* mesh_name, const char* texture_name, const char* density_img_name) : Entitymesh() {
	meshtype = INSTANCED;
	mesh = Mesh::Get(mesh_name);
	texture = Texture::Get(texture_name);
	shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture_light.fs");
	density_img = new Image();
	density_img->loadPNG(density_img_name);
	density_img->flipY(); //flip the png file (always is flipped)
	random_positions();
	//render_this = true;
	//type = FOREST;
}

InstancedMesh::InstancedMesh(const char* mesh_name, const char* texture_name) : Entitymesh() {
	meshtype = INSTANCED;
	mesh = Mesh::Get(mesh_name);
	texture = Texture::Get(texture_name);
	shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture_light.fs");
	//render_this = true;
}

InstancedMesh::~InstancedMesh(){

}

//de prueba, borrar
void InstancedMesh::random_positions() {
	Matrix44 m;
	for (int x = 0; x < 256 * 20; x += 50) {
		for (int z = 0; z < 256 * 20; z += 50) {

			if ((rand() % 100) < 70)
				continue;

			Color pixel = density_img->getPixel(x / 2, z / 2);
			if (pixel.b < (random() * 255))
				continue;

			m.setTranslation(x, 0, z);
			float s = 1 + random() * 2;
			m.scale(s, s, s);
			transforms.push_back(m);
		}
	}
}

void InstancedMesh::render() {

	if (!shader)
		return;

	//get the last camera that was activated
	Camera* camera = Camera::current;
	Light* light = Scene::instance->main_light;//getting the light

	shader->enable();

	render_basic_shader(camera, Matrix44(), light);
	
	//render the mesh using the shader
	mesh->renderInstanced(GL_TRIANGLES, &transforms[0], transforms.size());

	//disable the shader after finishing rendering
	shader->disable();
}

void InstancedMesh::update(float dt) {

}