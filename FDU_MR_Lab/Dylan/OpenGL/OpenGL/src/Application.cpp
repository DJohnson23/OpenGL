#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

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
#include "Camera.h"
#include "Eyes.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


static const int SCREEN_WIDTH = 1920;
static const int SCREEN_HEIGHT = 1080;

static const int RIGHT_EYE_MONITOR = 1;
static const float PI = 3.141592f;
static const float PI2 = PI * 2;

static const float ROT_SPEED = PI / 4.0f;
static const float CAM_SPEED = 1.0f;
static const float IPD_SPEED = 0.5f;

static const float HEIGHT_SPEED = 0.5f;
static const float RAD_SPEED = 0.1f;
static const float LENGTH_SPEED = 0.5f;

static const int WEBCAM_FRAME_WIDTH = 640;
static const int WEBCAM_FRAME_HEIGHT = 360;

static const int RIGHT_EYE_CAM = 0;
static const int LEFT_EYE_CAM = 2;

float deltaTime = 0;

void updateValue(float& value, float speed)
{
    if (Input::GetKey(GLFW_KEY_LEFT_SHIFT) || Input::GetKey(GLFW_KEY_RIGHT_SHIFT))
        speed /= 2.0f;
    else if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) || Input::GetKey(GLFW_KEY_RIGHT_CONTROL))
        speed *= 2.0f;

    value += speed * deltaTime;
}

int main(void) 
{

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    std::cout << "Monitor Count:" << monitorCount << std::endl;

    int leftPosition = 0, winWidth = 0, winHeight = 0;

    for (int i = 0; i < monitorCount; i++)
    {
        GLFWmonitor* curMon = monitors[i];

        int xPos, yPos;
        glfwGetMonitorPos(curMon, &xPos, &yPos);

        const GLFWvidmode* mode = glfwGetVideoMode(curMon);
        winWidth += mode->width;

        if (mode->height > winHeight)
            winHeight = mode->height;

        if (xPos < leftPosition)
            leftPosition = xPos;
    }


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(winWidth, winHeight, "Window", NULL, NULL);
    glfwSetWindowPos(window, leftPosition, 0);

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
        float radius = 0.1f, height = 3.0f, squareLength = 2.0f;
        Mesh doublePyramid = Mesh::DoublePyramid(squareLength, height, radius);

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

        Eyes eyes;

        Camera uiCam;
        uiCam.orthographic = true;

        glm::mat4 camMat = uiCam.viewMat();

        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                std::cout << camMat[r][c] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
        
        glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
        float rotationAngle = 0.0f;
        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);


        glm::mat4 tranMat = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

        glm::mat4 model = tranMat * rotMat * scaleMat;

        Shader basicShader("res/shaders/Basic.shader");
        basicShader.Bind();
        basicShader.SetUniform4f("u_Color", 0.0f, 1.0f, 0.0f, 1.0f);
        basicShader.SetUniformMat4f("Model", model);

        Texture rightBGTex("res/textures/Apple.png");

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

        Shader uiShader("res/shaders/ui.shader");
        uiShader.Bind();
        uiShader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        uiShader.SetUniformMat4f("Model", orthoModel);
        uiShader.SetUniformMat4f("View", orthoView);
        uiShader.SetUniformMat4f("Projection", orthoProj);

        uiShader.SetUniform1i("u_Texture", 0);

        uiShader.Unbind();

        Renderer renderer(window);

        bool calibrating = false;
        bool rotating = false;

        float r = 0.0f;
        float increment = 0.05f;

        long lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        cv::VideoCapture rightEyeCapture(RIGHT_EYE_CAM);
        rightEyeCapture.set(cv::CAP_PROP_FRAME_WIDTH, WEBCAM_FRAME_WIDTH);
        rightEyeCapture.set(cv::CAP_PROP_FRAME_HEIGHT, WEBCAM_FRAME_HEIGHT);

        glm::mat4 bgProj = glm::ortho(0.0f, (float)winWidth, 0.0f, (float)winHeight, -1.0f, 1.0f);

        Mesh bgRight = Mesh::UIRectangle(glm::vec2(SCREEN_WIDTH, 0), glm::vec2(winWidth, winHeight));
        Shader bgShader(uiShader);
        bgShader.Bind();
        bgShader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        bgShader.SetUniformMat4f("Model", orthoModel);
        bgShader.SetUniformMat4f("View", orthoView);
        bgShader.SetUniformMat4f("Projection", bgProj);
        bgShader.Unbind();

        cv::Mat rightEyeImg;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {

            if (rightEyeCapture.isOpened())
            {
                bool grabbed = rightEyeCapture.grab();
                bool retrieved = rightEyeCapture.retrieve(rightEyeImg);

                if (grabbed && retrieved && !rightEyeImg.empty())
                {
                    cv::flip(rightEyeImg, rightEyeImg, 0);
                    cv::cvtColor(rightEyeImg, rightEyeImg, cv::COLOR_BGR2RGB);

                    rightBGTex.setData(rightEyeImg);
                    rightBGTex.Bind();
                    bgShader.Bind();
                    bgShader.SetUniform1i("u_Texture", 0);
                    bgShader.Unbind();
                }
            }

            /* Render here */
            renderer.Clear();

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
                glfwSetWindowShouldClose(window, 1);
            }

            if (rotating)
            {
                rotationAngle += PI * deltaTime;
            }
            else if(Input::GetKey(GLFW_KEY_LEFT))
            {
                updateValue(rotationAngle, -ROT_SPEED);
            }
            else if (Input::GetKey(GLFW_KEY_RIGHT))
            {
                updateValue(rotationAngle, ROT_SPEED);
            }

            bool meshChanged = false;

            if (Input::GetKey(GLFW_KEY_W))
            {
                updateValue(height, HEIGHT_SPEED);
                meshChanged = true;
            }
            else if (Input::GetKey(GLFW_KEY_S))
            {
                updateValue(height, -HEIGHT_SPEED);

                if (height < 0.1f)
                    height = 0.1f;

                meshChanged = true;
            }

            if (Input::GetKey(GLFW_KEY_E))
            {
                updateValue(radius, RAD_SPEED);
                meshChanged = true;
            }
            else if (Input::GetKey(GLFW_KEY_Q))
            {
                updateValue(radius, -RAD_SPEED);

                if (radius < 0.01f)
                    radius = 0.01f;

                meshChanged = true;
            }

            if (Input::GetKey(GLFW_KEY_D))
            {
                updateValue(squareLength, LENGTH_SPEED);
                meshChanged = true;
            }
            else if (Input::GetKey(GLFW_KEY_A))
            {
                updateValue(squareLength, -LENGTH_SPEED);

                if (squareLength < 0.1f)
                    squareLength = 0.1f;

                meshChanged = true;
            }

            if (meshChanged)
            {
                doublePyramid = Mesh::DoublePyramid(squareLength, height, radius);
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
                updateValue(eyes.center.z, -CAM_SPEED);

                if (eyes.center.z < 0)
                    eyes.center.z = 0;
            }
            else if (Input::GetKey(GLFW_KEY_DOWN))
            {
                updateValue(eyes.center.z, CAM_SPEED);
            }

            if (Input::GetKey(GLFW_KEY_I))
            {
                updateValue(eyes.ipd, IPD_SPEED);
            }
            else if (Input::GetKey(GLFW_KEY_U))
            {
                updateValue(eyes.ipd, -IPD_SPEED);

                if (eyes.ipd < 0.1f)
                    eyes.ipd = 0.1f;
            }

            //rotationAngle = 0.785f;

            basicShader.Bind();

            rotMat = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
            model = tranMat * rotMat * scaleMat;

            basicShader.SetUniformMat4f("Model", model);

            basicShader.Unbind();


            //renderer.StereoscopicDraw(background, uiShader, uiCam, uiCam);
            renderer.MonoscopicDraw(bgRight, bgShader);

            renderer.ClearDepthBuffer();

            renderer.StereoscopicDraw(doublePyramid, basicShader, eyes);


            // ---------- Calibration lines ----------

            renderer.ClearDepthBuffer();
            if (calibrating)
            {
                renderer.StereoscopicDraw(line1, lineShader, uiCam, uiCam);
                renderer.StereoscopicDraw(line2, lineShader, uiCam, uiCam);
                         
                renderer.StereoscopicDraw(circle1, lineShader, uiCam, uiCam);
                renderer.StereoscopicDraw(circle2, lineShader, uiCam, uiCam);
                renderer.StereoscopicDraw(circle3, lineShader, uiCam, uiCam);
                renderer.StereoscopicDraw(circle4, lineShader, uiCam, uiCam);
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