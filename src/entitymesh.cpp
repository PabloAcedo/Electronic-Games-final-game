#include "entitymesh.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"
#include "entity.h"
#include "light.h"
#include "framework.h"
#include "scene.h"


Entitymesh::Entitymesh() : Entity(){
	color = Vector4(1, 1, 1, 1);
	texture_tiling = 1.0f;
	shader = Shader::Get("data/shaders/basic_light.vs", "data/shaders/texture_light.fs");
	light_trick = false;
	type = MESH;
}

Entitymesh::~Entitymesh(){
	
}

Entitymesh::Entitymesh(const char* mesh_name, const char* texture_name) : Entity() {

	shader = Shader::Get("data/shaders/basic_light.vs", "data/shaders/texture_light.fs");
	texture = Texture::Get(texture_name);
	mesh = Mesh::Get(mesh_name);
	color = Vector4(1, 1, 1, 1);
	texture_tiling = 1.0f;
	light_trick = false;
	type = MESH;
	
}


//uploading values to the shader
void Entitymesh::render_basic_shader(Camera* camera, Matrix44 model, Light* light) {

	shader->setUniform("u_color", color);
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setTexture("u_texture", texture,0);
	shader->setUniform("u_texture_tiling", texture_tiling);
	shader->setUniform("u_light_pos", light->position);
	shader->setUniform("u_light_trick", Vector3(1,1,1));

	if(light_trick == true)
		shader->setUniform("u_light_trick", Vector3(-1, 1, -1));

	shader->setUniform("u_light_color_diffuse", light->diffuse_color);
	shader->setUniform("u_light_color_specular", light->specular_color);
	shader->setUniform("u_camera_eye", camera->eye);
	shader->setUniform("u_sky_color", Scene::instance->skycolor);
	shader->setUniform("u_far_plane", Scene::instance->far_plane);
}

void Entitymesh::render() {

	if (!shader)
		return;

	//get the last camera that was activated
	Camera* camera = Camera::current;
	Light* light = Scene::instance->main_light;//getting the light
	Matrix44 model = getGlobalMatrix();

	//testing if the camera sees the object
	BoundingBox global_box = transformBoundingBox(model,mesh->box);
	if (!camera->testBoxInFrustum(global_box.center, global_box.halfsize))
		return;

	//enable shader and pass uniforms
	shader->enable();

	render_basic_shader(camera, model, light);
	
	//render the mesh using the shader
	mesh->render(GL_TRIANGLES,-1); 

	//disable the shader after finishing rendering
	shader->disable();

	for (int i = 0; i < children.size(); i++)
		if(children[i]->show)
			children[i]->render();  //repeat for every child

}

void Entitymesh::update(double dt) {

}
