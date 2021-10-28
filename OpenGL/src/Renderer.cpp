#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <math.h>
#include <memory>

const float Renderer::PI = 3.141592f;

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

void Renderer::StereoscopicDraw(const Mesh& m, Shader& shader, const Camera cam, float ipd) const
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float halfIpd = ipd / 2;

    shader.Bind();

    glm::vec3 forward = cam.camTarget - cam.camPos;
    glm::vec3 right = glm::normalize(glm::cross(forward, cam.upVector));

    glm::vec3 camTransform = right * halfIpd;

    glm::mat4 view = glm::lookAt(
        cam.camPos - camTransform, // Camera is at (4,3,3) in World Space
        cam.camTarget - camTransform, // looks at origin
        cam.upVector  // Up Vector
    );
    shader.SetUniformMat4f("View", view);

    GLCall(glViewport(0, 0, width / 2.0f, height));
    Draw(m, shader);

    view = glm::lookAt(
        cam.camPos + camTransform, // Camera is at (4,3,3) in World Space
        cam.camTarget + camTransform, // looks at origin
        cam.upVector  // Up Vector
    );
    shader.SetUniformMat4f("View", view);

    GLCall(glViewport(width / 2.0f, 0, width / 2.0f, height));
    Draw(m, shader);
}

void Renderer::MonoscopicDraw(const Mesh& m, const Shader& shader) const
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    GLCall(glViewport(0, 0, width, height));
    Draw(m, shader);
}

glm::vec2* Renderer::LinePoints(glm::vec2& start, glm::vec2& end, float thickness) const
{
    float halfThick = thickness / 2;
    glm::vec2 dir = glm::normalize(end - start);
    glm::vec2 perp = glm::vec2(-dir.y, dir.x);

    glm::vec2 points[] =
    {
        start + (perp * halfThick),
        end + (perp * halfThick),
        end - (perp * halfThick),
        start - (perp * halfThick)
    };

    return points;
}

void Renderer::DrawLine(glm::vec2& start, glm::vec2& end, float thickness, const Shader& shader) const
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    GLCall(glViewport(0, 0, width, height));

    glm::vec2* points = LinePoints(start, end, thickness);

    float verts[] =
    {
        points[0].x, points[0].y, 0.0f,
        points[1].x, points[1].y, 0.0f,
        points[2].x, points[2].y, 0.0f,
        points[3].x, points[3].y, 0.0f
    };

    VertexArray va;
    VertexBuffer vb(verts, 4 * 3 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    const unsigned int tris[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    // Index Buffer object
    IndexBuffer ib(tris, 6);

    Draw(va, ib, shader);
}

void Renderer::DrawCircle(glm::vec2& center, float radius, float thickness, int segments, const Shader& shader) const
{
    float halfThick = thickness / 2.0f;

    int numVerts = segments * 2;

    float* verts = new float[numVerts * 3];
    unsigned int* tris = new unsigned int[segments * 2 * 3];

    int v = 0;
    for (int i = 0; i < segments; i++)
    {
        float theta = (float)i / segments * 2 * PI;

        float y = center.y + radius * sinf(theta);
        float x = center.x + radius * cosf(theta);

        glm::vec2 p = glm::vec2(x, y);
        glm::vec2 dir = glm::normalize(center - p);

        glm::vec2 close = p + dir * halfThick;
        glm::vec2 far = p - dir * halfThick;

        verts[v++] = close.x;
        verts[v++] = close.y;
        verts[v++] = 0.0f;
               
        verts[v++] = far.x;
        verts[v++] = far.y;
        verts[v++] = 0.0f;
    }

    int t = 0;
    for (int i = 0; i < segments - 1; i++)
    {
        int v0 = i * 2;

        tris[t++] = v0 + 0;
        tris[t++] = v0 + 1;
        tris[t++] = v0 + 3;

        tris[t++] = v0 + 0;
        tris[t++] = v0 + 3;
        tris[t++] = v0 + 2;
    }

    tris[t++] = numVerts - 2;
    tris[t++] = 1;
    tris[t++] = 0;

    tris[t++] = numVerts - 2;
    tris[t++] = numVerts - 1;
    tris[t++] = 1;

    VertexArray va;
    VertexBuffer vb(verts, numVerts * 3 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    // Index Buffer object
    IndexBuffer ib(tris, numVerts * 3);

    Draw(va, ib, shader);
}

Renderer::Renderer(GLFWwindow* win)
{
    window = win;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const Mesh& m, const Shader& shader) const
{
    const int FLOATS_PER_VERT = 8;

    float* verts = new float[m.positions.size() * FLOATS_PER_VERT];

    for (int i = 0; i < m.positions.size(); i++)
    {
        unsigned int index = i * FLOATS_PER_VERT;

        verts[index + 0] = m.positions[i].x;
        verts[index + 1] = m.positions[i].y;
        verts[index + 2] = m.positions[i].z;

        verts[index + 3] = m.texCoords[i].x;
        verts[index + 4] = m.texCoords[i].y;

        verts[index + 5] = m.normals[i].x;
        verts[index + 6] = m.normals[i].y;
        verts[index + 7] = m.normals[i].z;
    }

    VertexArray va;
    VertexBuffer vb(verts, m.positions.size() * FLOATS_PER_VERT * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    // Index Buffer object
    IndexBuffer ib(m.triangles.data(), m.triangles.size());

    Draw(va, ib, shader);
}
