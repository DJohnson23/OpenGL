#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const int SCREEN_WIDTH = 960;
static const int SCREEN_HEIGHT = 540;

int main(void) 
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        Mesh cubeMesh = Mesh::Cube();
        Mesh planeMesh = Mesh::Plane();
        //Mesh dollMesh = Mesh::LoadMesh("res/meshes/Doll.obj");
        Mesh octahedron = Mesh::LoadMesh("res/meshes/OctahedronWire.obj");
        

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDepthFunc(GL_LESS));
        

        Camera cam;
        cam.camPos = glm::vec3(0, 5, 10);
        cam.camTarget = glm::vec3(0, 0, 0);
        cam.upVector = glm::vec3(0, 1, 0);

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / 2.0f / (float)SCREEN_HEIGHT, 0.1f, 200.0f);
        //glm::mat4 proj = glm::perspective<float>(60.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 150.0f);
        glm::mat4 view = glm::lookAt(
            cam.camPos, // Camera World Space position
            cam.camTarget, // looks at origin
            cam.upVector  // Up Vector
        );
        
        glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
        float rotationAngle = 0.0f;
        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);


        glm::mat4 tranMat = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

        glm::mat4 model = tranMat * rotMat * scaleMat;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
        shader.SetUniformMat4f("Model", model);
        shader.SetUniformMat4f("View", view);
        shader.SetUniformMat4f("Projection", proj);
        shader.SetUniform3f("viewPos", cam.camPos);

        Texture texture("res/textures/Apple.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);
        
        shader.Unbind();

        Renderer renderer(window);

        float ipd = 0.64f;

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            //shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
            shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

            rotationAngle += 0.05f;

            while (rotationAngle > 360)
                rotationAngle -= 360;

            rotationAngle = 0.785f;

            rotMat = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
            model = tranMat * rotMat * scaleMat;

            shader.SetUniformMat4f("Model", model);


            //renderer.StereoscopicDraw(cubeMesh, shader, cam, ipd);
            renderer.StereoscopicDraw(octahedron, shader, cam, ipd);

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}