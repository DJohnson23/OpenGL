#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Mesh.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

struct Camera 
{
    glm::vec3 camPos;
    glm::vec3 camTarget;
    glm::vec3 upVector;
};

class Renderer
{
private:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Draw(Mesh& m, const Shader& shader) const;
    GLFWwindow* window;
public:
    Renderer(GLFWwindow* win);
    void Clear() const;
    void StereoscopicDraw(Mesh& m, Shader& shader, const Camera cam, float ipd) const;
    void MonoscopicDraw(Mesh& m, const Shader& shader) const;
};