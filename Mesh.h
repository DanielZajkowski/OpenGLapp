#pragma once

#include <GL/glew.h>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
	void ClearMesh();

private:
	// VAO - Vertex Array Objects - defines what data a vertex has (position, colour, texture, normals, etc)
	// VBO - Vertex Buffer Objects - defines the data itself
	// IBO - Index/Element Buffer Objects - defines the indices the OpenGL uses to decide what vertices to draw.
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};


