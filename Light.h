#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity);
	~Light();

protected:
	glm::vec3 colour;
	// Ambient lighting - Simulates light bouncing off other objects. Not required a direction.
	GLfloat ambientIntensity;
	// Diffuse lighting - Simulates the drop-off of lighting as angle of lighting becomes more shallow. Required a direction.
	GLfloat diffuseIntensity;
};

