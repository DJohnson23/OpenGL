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
    static const float PI;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Draw(const Mesh& m, const Shader& shader) const;
    glm::vec2* LinePoints(glm::vec2& start, glm::vec2& end, float thickness) const;
    GLFWwindow* window;
public:
    Renderer(GLFWwindow* win);
    void Clear() const;
    void StereoscopicDraw(const Mesh& m, Shader& shader, const Camera cam, float ipd) const;
    void MonoscopicDraw(const Mesh& m, const Shader& shader) const;
    void DrawLine(glm::vec2& start, glm::vec2& end, float thickness, const Shader& shader) const;
    void DrawCircle(glm::vec2& center, float radius, float thickness, int segments, const Shader& shader) const;
};