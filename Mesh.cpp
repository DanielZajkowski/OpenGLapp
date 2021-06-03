#include "Mesh.h"

Mesh::Mesh()
{
    VAO = 0;
    VBO = 0;
    IBO = 0;
    indexCount = 0;
}

Mesh::~Mesh()
{
    ClearMesh();
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
    indexCount = numOfIndices;

    // Generate a VAO ID
    glGenVertexArrays(1, &VAO);
    // Bind the VAO with that ID
    glBindVertexArray(VAO);

    // Creating a buffer for IBO
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

    // Generate a VBO ID
    glGenBuffers(1, &VBO);
    // Bind the VBO with that ID (now working on the choosen VBO attached to the chosen VAO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Attach the vertex data to that VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    // Define the Attribute Pointer formatting
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
    // Enable the Attribute Pointer
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices) * 8, (void*)(sizeof(vertices[0]) * 3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices) * 8, (void*)(sizeof(vertices[0]) * 5));
    glEnableVertexAttribArray(2);

    // Unbind the VAO and VBO, ready for the next object to be bound
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderMesh()
{
    // Active Shader Program to use
    glBindVertexArray(VAO);
    // Bind IBO of object you want to draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    // Calling glDrawElements, which initiates the rest of the pipeline, drawing by the elements
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    // Unbinding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
    if (IBO != 0)
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}
