#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, 
		  GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity);
	~Light();

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
				  GLuint diffuseIntensityLocation, GLuint directionLocation);

private:
	// Ambient lighting
	glm::vec3 colour;
	GLfloat ambientIntensity;

	// Diffuse lighting
	glm::vec3 direction;
	GLfloat diffuseIntensity;
};

