#include "Input.h"
#include <iostream>

std::vector<int> Input::lastPressed;
GLFWwindow* Input::window;

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
		lastPressed.push_back(key);
}

bool Input::GetKeyDown(int key)
{
	for (int pressed : lastPressed)
	{
		if (pressed == key)
			return true;
	}

	return false;
}

bool Input::GetKey(int key)
{
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void Input::Initialize(GLFWwindow* _window)
{
	window = _window;
	glfwSetKeyCallback(_window, KeyCallback);
}

void Input::Update()
{
	lastPressed.clear();
}
