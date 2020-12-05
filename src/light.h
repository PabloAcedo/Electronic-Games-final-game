#ifndef LIGHT_H
#define LIGHT_H

#include "framework.h"

//This class contains all the info about the properties of the light
class Light
{
public:
	Vector3 position; //where is the light
	Vector3 diffuse_color; //the amount (and color) of diffuse
	Vector3 specular_color; //the amount (and color) of specular

	Light();
};
#endif
