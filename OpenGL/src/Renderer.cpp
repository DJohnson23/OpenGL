#include "Renderer.h"
#include "VertexBufferLayout.h"

#include <iostream>

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

    const unsigned int* tris = m.triangles.data();

    // Index Buffer object
    IndexBuffer ib(m.triangles.data(), m.triangles.size());

    Draw(va, ib, shader);
}
