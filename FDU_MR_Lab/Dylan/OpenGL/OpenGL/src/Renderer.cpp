#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <math.h>
#include <memory>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }

    return true;
}

void Renderer::StereoscopicDraw(Mesh& m, Shader& shader, const Camera& leftCam, const Camera& rightCam) const
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    shader.Bind();

    shader.SetUniformMat4f("View", leftCam.viewMat());
    GLCall(glViewport(0, 0, width / 2.0f, height));
    Draw(m, shader);

    shader.SetUniformMat4f("View", rightCam.viewMat());
    GLCall(glViewport(width / 2.0f, 0, width / 2.0f, height));
    Draw(m, shader);

    shader.Unbind();
}

void Renderer::MonoscopicDraw(Mesh& m, const Shader& shader) const
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    GLCall(glViewport(0, 0, width, height));
    Draw(m, shader);
}

Renderer::Renderer(GLFWwindow* win)
{
    window = win;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::ClearDepthBuffer() const
{
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(Mesh& m, const Shader& shader) const
{
    float* verts = m.getVerts();

    VertexArray va;
    VertexBuffer vb(verts, m.positions.size() * Mesh::FLOATS_PER_VERT * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    // Index Buffer object
    IndexBuffer ib(m.triangles.data(), m.triangles.size());

    Draw(va, ib, shader);
}
