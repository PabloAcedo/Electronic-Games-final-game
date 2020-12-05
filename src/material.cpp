#include "material.h"



Material::Material()
{
	ambient.set(8, 8, 8); //reflected ambient light
	diffuse.set(10, 10, 10); //reflected diffuse light
	specular.set(1, 1, 1); //reflected specular light
	shininess = 30.0; //glosiness coefficient (plasticity)
}


