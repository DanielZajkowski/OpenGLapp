#pragma once

#include <GL/glew.h>

class Material
{
public:
	Material();
	Material(GLfloat sIntensity, GLfloat shine);
	~Material();

	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

private:
	// Define how much of the light source the the object will actually absorb as opposed to reflect
	GLfloat specularIntensity;
	// Define how smooth the surface is going to be portayed as. Sometime it is called a specular power
	GLfloat shininess;
};

