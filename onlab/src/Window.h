#pragma once
#include "Scene.h"
#include "TransparencyMode.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <memory>
#include <iostream>

class Window {
	int windowWidth = 1920;					// TODO: move to Settings or somesuch
	int windowHeight = 1080;				// -//-
	TransparencyMethod renderingTransparencyMode;	// -//-
	bool mouseDragState;
	double mousePrevX = 0.0;
	double mousePrevY = 0.0;

	GLFWwindow* window;
	std::unique_ptr<Scene> scene;
public:
	Window() = default;
	~Window();
	bool create();
	void renderLoop();
	void resize_callback(int width, int height);
	void key_callback(int key, int scancode, int action, int mods);
	void mouseMove_callback(double xpos, double ypos);
	void mousePress_callback(int button, int action, int mods);
	void mouseScroll_callback(double xoffset, double yoffset);
private:
	glm::vec2 screen2NDC(double xpos, double ypos);
	//static callback wrappers

	static void error_callback(int error, const char* desc) {
		std::cerr << "GLFW error[" << error << "]: " << desc << std::endl;
	}
	static void resize_callback(GLFWwindow* window, int width, int height) {
		Window* mainWindow = (Window*)glfwGetWindowUserPointer(window);
		mainWindow->resize_callback(width, height);
	}
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Window* mainWindow = (Window*)glfwGetWindowUserPointer(window);
		mainWindow->key_callback(key, scancode, action, mods);
	}
	static void mouseMove_callback(GLFWwindow* window, double xpos, double ypos) {
		Window* mainWindow = (Window*)glfwGetWindowUserPointer(window);
		mainWindow->mouseMove_callback(xpos, ypos);
	}
	static void mousePress_callback(GLFWwindow* window, int button, int action, int mods) {
		Window* mainWindow = (Window*)glfwGetWindowUserPointer(window);
		mainWindow->mousePress_callback(button, action, mods);
	}
	static void mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		Window* mainWindow = (Window*)glfwGetWindowUserPointer(window);
		mainWindow->mouseScroll_callback(xoffset, yoffset);
	}
};