#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int windowWidth = 640;
const int windowHeight = 480;

static void error_callback(int error, const char* desc) {
	fprintf(stderr, "Error: %s\n", desc);
}

static void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		fprintf(stdout, "G key pressed\n");
	}
}

static void mouseMove_callback(GLFWwindow* window, double xpos, double ypos) {
	double Cx = 2.0f * xpos / windowWidth - 1;
	double Cy = 1.0f - 2.0f * ypos / windowHeight;
	fprintf(stdout, "Mouse moved to : (%f, %f)\n", Cx, Cy);
}

static void mousePress_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			fprintf(stdout, "Left pressed\n");
			break;
		case GLFW_MOUSE_BUTTON_2:
			fprintf(stdout, "Right pressed\n");
			break;
		default:
			fprintf(stdout, "Other mouse button pressed\n");
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			fprintf(stdout, "Left released\n");
			break;
		case GLFW_MOUSE_BUTTON_2:
			fprintf(stdout, "Right released\n");
			break;
		default:
			fprintf(stdout, "Other mouse button released\n");
		}
	}
}

//static void mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset);