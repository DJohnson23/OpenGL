#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <map>
#include <vector>

class Input
{
private:
	static void KeyCallback(GLFWwindow* _window, int key, int scancode, int action, int mods);
	static std::vector<int> lastPressed;
	static GLFWwindow* window;

public:
	static bool GetKeyDown(int key);
	static bool GetKey(int key);
	static void Initialize(GLFWwindow* _window);
	static void Update();
};

#endif // INPUT_H

