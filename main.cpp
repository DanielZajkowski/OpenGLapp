#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model boat;
Model seahawk;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat seahawkAngle = 0.0f;
GLfloat boatAngle = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";
// Fragement Shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indicesCount, GLfloat* vertices, unsigned int verticesCount, 
                        unsigned int vLength, unsigned int normalOffset)
{
    for (size_t i = 0; i < indicesCount; i += 3)
    {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;

        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v1, v2);

        normal = glm::normalize(normal);

        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;

        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    for (size_t i = 0; i < verticesCount / vLength; i++)
    {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
}

void CreateObjects()
{
    // Indices which are going to tell the GPU which points place in which order
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
    //   x      y      z      u     v      nx    ny    nz
        -1.0f, -1.0f, -0.6f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,   // bottom left  - 0 // Black colour
         0.0f, -1.0f,  1.0f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f,   // background   - 1 // Blue
         1.0f, -1.0f, -0.6f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,   // bottom right - 2 // Red
         0.0f,  1.0f,  0.0f,  0.5f, 1.0f,  0.0f, 0.0f, 0.0f    // top          - 3 // Green
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3,
    };

    GLfloat floorVertices[] = {
        //   x      y      z      u     v      nx    ny    nz
            -10.0f, 0.0f, -10.0f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,   // bottom left  - 0 // Black colour
             10.0f, -1.0f,  -10.0f,  10.0f, 0.0f,  0.0f, -1.0f, 0.0f,   // background   - 1 // Blue
             -10.0f, -1.0f, 10.0f,  0.0f, 10.0f,  0.0f, -1.0f, 0.0f,   // bottom right - 2 // Red
             10.0f,  0.0f,  10.0f,  10.0f, 10.0f,  0.0f, -1.0f, 0.0f    // top          - 3 // Green
    };

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);
    
    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj3);
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);

    directionalShadowShader = Shader();
    directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
}

void RenderScene()
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[0]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[1]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[2]->RenderMesh();

    boatAngle += 0.1f;
    if (boatAngle > 360.0f)
    {
        boatAngle = 0.1f;
    }

    model = glm::mat4(1.0f);
    model = glm::rotate(model, -boatAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-4.0f, -0.5f, 0.0f));
    model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, 20.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    boat.RenderModel();

    seahawkAngle += 0.1f;
    if (seahawkAngle > 360.0f)
    {
        seahawkAngle = 0.1f;
    }

    model = glm::mat4(1.0f);
    model = glm::rotate(model, seahawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-5.0f, 1.0f, 0.0f));
    model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    //model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    seahawk.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
    directionalShadowShader.UseShader();
    // Setting viewport to the same dimension as framebuffer
    glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

    light->GetShadowMap()->Write();
    // Clearing before Shadow Map
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = directionalShadowShader.GetModelLocation();
    glm::mat4 lightTransform = light->CalculateLightTransform();
    directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

    RenderScene();
    // Attaching default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    shaderList[0].UseShader();

    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformEyePosition = shaderList[0].GetEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
    uniformShininess = shaderList[0].GetShininessLocation();

    glViewport(0, 0, 1366, 768);

    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Combing two buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, camera.GetCameraPosition().x, camera.GetCameraPosition().y, camera.GetCameraPosition().z);

    shaderList[0].SetDirectionLight(&mainLight);
    shaderList[0].SetPointLights(pointLights, pointLightCount);
    shaderList[0].SetSpotLights(spotLights, spotLightCount);

    glm::mat4 lightTransform = mainLight.CalculateLightTransform();
    shaderList[0].SetDirectionalLightTransform(&lightTransform);

    mainLight.GetShadowMap()->Read(GL_TEXTURE1);
    shaderList[0].SetTexture(0);
    shaderList[0].SetDirectionalShadowMap(1);
    
    // Adding spotlight to camera
    glm::vec3 lowerLight = camera.GetCameraPosition();
    lowerLight.y -= 0.3f;
    //spotLights[0].SetFlash(lowerLight, camera.GetCameraDirection());

    RenderScene();
}

int main()
{
    mainWindow = Window(1366, 768);
    mainWindow.Initialise();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();

    shinyMaterial = Material(1.0f, 32.0f);
    dullMaterial = Material(0.3f, 4.0f);

    boat = Model();
    boat.LoadModel("Models/Boat.obj");

    seahawk = Model();
    seahawk.LoadModel("Models/Seahawk.obj");

    mainLight = DirectionalLight(2048, 2048,
                                1.0f, 1.0f, 1.0f, 
                                0.1f, 0.3f,
                                0.0f, -15.0f, -10.0f);

    
    pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
                                0.0f, 0.1f,
                                0.0f, 0.0f, 0.0f,
                                0.3f, 0.2f, 0.1f);
    pointLightCount++;
    pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
                                0.0f, 0.1f,
                                -4.0f, 2.0, 0.0f,
                                0.3f, 0.1f, 0.1f);
    pointLightCount++;

    
    spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
                              0.0f, 2.0f,
                              0.0f, 0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f,
                              1.0f, 0.2f, 0.1f,
                              20.0f);
    spotLightCount++;
    spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.5f, 0.0f,
        -100.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        20.0f);
    spotLightCount++;

    glm::mat4 projection = glm::perspective(45.0f, mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight(), 0.1f, 100.0f);

    // Loop until window closed
    while (!mainWindow.GetShouldClose())
    {
        GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
        deltaTime = now - lastTime;  // (now - lastTime)*1000/SDL_PerformanceFrequency();
        lastTime = now;

        // Get + Handle user input events
        glfwPollEvents();

        camera.KeyControl(mainWindow.GetsKeys(), deltaTime);
        camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());

        // Render to the scene, not to the actual screen
        DirectionalShadowMapPass(&mainLight);
        RenderPass(projection, camera.CalculateViewMatrix());

        glUseProgram(0);

        mainWindow.SwapBuffers();
    }

    return 0;
}