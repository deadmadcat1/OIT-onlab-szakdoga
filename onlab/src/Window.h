#pragma once
#include "Scene.h"
#include "TransparencyMode.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <memory>

class Window {
	static const int windowWidth = 640;					// TODO: move to Settings or somesuch
	static const int windowHeight = 480;				// -//-
	static TransparencyMode renderingTransparencyMode;	// -//-

	std::unique_ptr<GLFWwindow> window;
	std::unique_ptr<Scene> scene;

	static void resize_callback(GLFWwindow* window, int width, int height);
	static void error_callback(int error, const char* desc);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseMove_callback(GLFWwindow* window, double xpos, double ypos);
	static void mousePress_callback(GLFWwindow* window, int button, int action, int mods);
	static void mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset);
public:
	Window() = default;
	~Window();
	bool create();
	bool renderLoop();
	void resize_callback(int width, int height);
	void key_callback(int key, int scancode, int action, int mods);
	void mouseMove_callback(double xpos, double ypos);
	void mousePress_callback(int button, int action, int mods);
	void mouseScroll_callback(double xoffset, double yoffset);
};