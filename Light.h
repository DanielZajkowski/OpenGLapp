#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShadowMap.h"

class Light
{
public:
	Light();
	Light(GLuint shadowWidth, GLuint shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity);
	~Light();

	ShadowMap* GetShadowMap() { return shadowMap; }

protected:
	glm::vec3 colour;
	// Ambient lighting - Simulates light bouncing off other objects. Not required a direction.
	GLfloat ambientIntensity;
	// Diffuse lighting - Simulates the drop-off of lighting as angle of lighting becomes more shallow. Required a direction.
	GLfloat diffuseIntensity;

	// The view will be able to calculate dynamically based on where it is in the world
	glm::mat4 lightProj;

	ShadowMap* shadowMap;
};

