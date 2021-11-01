#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Input.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const int SCREEN_WIDTH = 1920;
static const int SCREEN_HEIGHT = 1080;

static const int RIGHT_EYE_MONITOR = 1;
static const float PI = 3.141592f;
static const float PI2 = PI * 2;

static const float ROT_SPEED = PI / 4.0f;
static const float CAM_SPEED = 0.1f;

int main(void) 
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    std::cout << "Monitor Count:" << monitorCount << std::endl;

    GLFWmonitor* rightMonitor = nullptr;

    if (monitorCount <= RIGHT_EYE_MONITOR)
        std::cout << "No right eye monitor" << std::endl;
    else
        rightMonitor = monitors[RIGHT_EYE_MONITOR];


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", rightMonitor, NULL);
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

    Input::Initialize(window);

    {
        Mesh cubeMesh = Mesh::Cube();
        Mesh planeMesh = Mesh::Plane();
        //Mesh dollMesh = Mesh::LoadMesh("res/meshes/Doll.obj");
        Mesh octahedron = Mesh::LoadMesh("res/meshes/OctahedronWire.obj");

        glm::vec3 leftMiddle = glm::vec3(0, SCREEN_HEIGHT / 2.0f, 0);
        glm::vec3 rightMiddle = glm::vec3(SCREEN_WIDTH, SCREEN_HEIGHT / 2.0f, 0);
        glm::vec3 topMiddle = glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT, 0);
        glm::vec3 bottomMiddle = glm::vec3(SCREEN_WIDTH / 2.0f, 0, 0);
        glm::vec3 screenCenter = glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0);

        float lineThickness = 1.0f;

        Mesh line1 = Mesh::Line(leftMiddle, rightMiddle, lineThickness);
        Mesh line2 = Mesh::Line(topMiddle, bottomMiddle, lineThickness);

        float circleSpacing = SCREEN_HEIGHT / 8.0f;
        Mesh circle1 = Mesh::Circle(screenCenter, circleSpacing * 1, lineThickness);
        Mesh circle2 = Mesh::Circle(screenCenter, circleSpacing * 2, lineThickness);
        Mesh circle3 = Mesh::Circle(screenCenter, circleSpacing * 3, lineThickness);
        Mesh circle4 = Mesh::Circle(screenCenter, circleSpacing * 4, lineThickness);
        

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDepthFunc(GL_LESS));
        

        Camera cam;
        cam.camPos = glm::vec3(0, -1.514, 10);
        cam.camTarget = glm::vec3(0, -1.514, 0);
        cam.upVector = glm::vec3(0, 1, 0);

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 200.0f);
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
        shader.SetUniform4f("u_Color", 0.0f, 1.0f, 0.0f, 1.0f);
        shader.SetUniformMat4f("Model", model);
        shader.SetUniformMat4f("View", view);
        shader.SetUniformMat4f("Projection", proj);
        shader.SetUniform3f("viewPos", cam.camPos);

        Texture texture("res/textures/Apple.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);
        
        shader.Unbind();

        glm::mat4 orthoModel = glm::mat4(1.0f);
        glm::mat4 orthoView = glm::mat4(1.0f);
        glm::mat4 orthoProj = glm::ortho(0.0f, SCREEN_WIDTH * 1.0f, 0.0f, SCREEN_HEIGHT * 1.0f, -1.0f, 1.0f);

        Shader lineShader("res/shaders/Line.shader");
        lineShader.Bind();
        lineShader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        lineShader.SetUniformMat4f("Model", orthoModel);
        lineShader.SetUniformMat4f("View", orthoView);
        lineShader.SetUniformMat4f("Projection", orthoProj);

        lineShader.Unbind();

        Renderer renderer(window);

        bool calibrating = false;
        bool rotating = false;

        float ipd = 0.64f;

        float r = 0.0f;
        float increment = 0.05f;

        long lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float deltaTime = 0;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            long newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = (newTime - lastTime) / 1000.0f;
            lastTime = newTime;

            //if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            if (Input::GetKeyDown(GLFW_KEY_SPACE))
            {
                rotating = !rotating;
            }

            if (Input::GetKeyDown(GLFW_KEY_TAB))
            {
                calibrating = !calibrating;
            }

            if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
            {
                exit(0);
            }

            /* Render here */
            renderer.Clear();

            shader.Bind();

            if (rotating)
            {
                rotationAngle += PI * deltaTime;
            }
            else if(Input::GetKey(GLFW_KEY_LEFT))
            {
                float speed = ROT_SPEED;

                if (Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
                    speed /= 2.0f;

                rotationAngle -= speed * deltaTime;
            }
            else if (Input::GetKey(GLFW_KEY_RIGHT))
            {
                float speed = ROT_SPEED;

                if (Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
                    speed /= 2.0f;

                rotationAngle += speed * deltaTime;
            }

            while (rotationAngle > PI2)
            {
                rotationAngle -= PI2;
            }

            while (rotationAngle < 0)
            {
                rotationAngle += PI2;
            }

            if (Input::GetKey(GLFW_KEY_UP))
            {
                float speed = CAM_SPEED;

                if (Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
                    speed /= 2.0f;

                cam.camPos.z -= speed;

                if (cam.camPos.z < 0)
                    cam.camPos.z = 0;
            }
            else if (Input::GetKey(GLFW_KEY_DOWN))
            {
                float speed = CAM_SPEED;

                if (Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
                    speed /= 2.0f;

                cam.camPos.z += speed;

                if (cam.camPos.z < 0)
                    cam.camPos.z = 0;
            }

            //rotationAngle = 0.785f;

            rotMat = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
            model = tranMat * rotMat * scaleMat;

            view = glm::lookAt(
                cam.camPos, // Camera World Space position
                cam.camTarget, // looks at origin
                cam.upVector  // Up Vector
            );

            shader.SetUniformMat4f("Model", model);
            shader.SetUniformMat4f("View", view);

            shader.Unbind();


            //renderer.StereoscopicDraw(cubeMesh, shader, cam, ipd);
            //renderer.StereoscopicDraw(octahedron, shader, cam, ipd);
            renderer.MonoscopicDraw(octahedron, shader);


            // ---------- Calibration lines ----------

            if (calibrating)
            {
                renderer.MonoscopicDraw(line1, lineShader);
                renderer.MonoscopicDraw(line2, lineShader);

                renderer.MonoscopicDraw(circle1, lineShader);
                renderer.MonoscopicDraw(circle2, lineShader);
                renderer.MonoscopicDraw(circle3, lineShader);
                renderer.MonoscopicDraw(circle4, lineShader);
            }

            // ---------- Calibration lines ----------

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            Input::Update();

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}