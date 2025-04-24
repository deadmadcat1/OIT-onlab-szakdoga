#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <iostream>
#include <cstdlib>
#include <math.h>

const int windowWidth = 640;
const int windowHeight = 480;

static void resize_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

static void error_callback(int error, const char* desc) {
	std::cerr << "GLFW error: " << desc << std::endl;
} 

static void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			std::cout << "Exiting..." << std::endl;
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_1:
			//TODO alphablend
			break;
		case GLFW_KEY_2:
			//TODO depthpeel
			break;
		case GLFW_KEY_3:
			//TODO moment
			break;
		case GLFW_KEY_4:
			//TODO wavelet
			break;
		default:
			break;
		}
	}
}

static void mouseMove_callback(GLFWwindow* window, double xpos, double ypos) {
	double Cx = 2.0f * xpos / windowWidth - 1;
	double Cy = 1.0f - 2.0f * ypos / windowHeight;
	std::cout << "Mouse moved to: ("<< Cx << ", "<<Cy<< ")" << std::endl;
}

static void mousePress_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			std::cout << "LMB pressed." << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_2:
			std::cout << "RMB pressed." << std::endl;
			break;
		default:
			std::cout << "Other mouse button pressed." << std::endl;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			std::cout << "LMB released." << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_2:
			std::cout << "RMB released." << std::endl;
			break;
		default:
			std::cout << "Other mouse button released." << std::endl;
		}
	}
}

//static void mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset);