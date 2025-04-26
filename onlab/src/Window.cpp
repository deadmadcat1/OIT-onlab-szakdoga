#include "Window.h"
#include <iostream>

bool Window::create() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = std::make_unique<GLFWwindow>(
		glfwCreateWindow(windowWidth, windowHeight, "OIT Onlab", NULL, NULL)
	);

	if (!window)
		return false;

	glfwMakeContextCurrent(window.get());
	glfwSetWindowUserPointer(window.get(), this);

	glewInit();

	std::cout << "OpenGL version: " << (const char*)glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glfwSetKeyCallback(window.get(), key_callback);
	glfwSetCursorPosCallback(window.get(), mouseMove_callback);
	glfwSetMouseButtonCallback(window.get(), mousePress_callback);
	glfwSetFramebufferSizeCallback(window.get(), resize_callback);
	//glfwSetScrollCallback(window.get(), mouseScroll_callback);

	glfwSwapInterval(1);
	scene = std::make_unique<Scene>();

	return (scene && scene->set());
}

bool Window::renderLoop() {
	while (!glfwWindowShouldClose(window.get()))
	{
		glClearColor(0.1f, 0.2f, 0.3f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static float tend = 0.0f;
		const float dt = 0.01f;
		float tstart = tend;
		tend = (float)glfwGetTime() / 1000.0f;
		for (float t = tstart; t < tend; t += dt) {
			float delta = fmin(dt, tend - t);
			scene->animate(delta);
		}

		scene->render(renderingTransparencyMode);

		glfwSwapBuffers(window.get());

		glfwPollEvents();
	}
}

Window::~Window()
{
	glfwDestroyWindow(window.get());
}

//callback implementations

void Window::resize_callback(int width, int height) {
	glViewport(0, 0, width, height);
}
void Window::key_callback(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			std::cout << "Exiting..." << std::endl;
			glfwSetWindowShouldClose(window.get(), true);
			break;
		case GLFW_KEY_1:
			renderingTransparencyMode = TransparencyMode::alphaBlend;
			std::cout << "Transparency mode: Alpha Blending" << std::endl;
			break;
		case GLFW_KEY_2:
			renderingTransparencyMode = TransparencyMode::depthPeel;
			std::cout << "Transparency mode: Dual Depth Peeling" << std::endl;
			break;
		case GLFW_KEY_3:
			renderingTransparencyMode = TransparencyMode::moment;
			std::cout << "Transparency mode: Moment-based" << std::endl;
			break;
		case GLFW_KEY_4:
			renderingTransparencyMode = TransparencyMode::wavelet;
			std::cout << "Transparency mode: Wavelet" << std::endl;
			break;
		default:
			break;
		}
	}
}
void Window::mouseMove_callback(double xpos, double ypos) {
	double Cx = 2.0f * xpos / windowWidth - 1;
	double Cy = 1.0f - 2.0f * ypos / windowHeight;
	std::cout << "Mouse moved to: (" << Cx << ", " << Cy << ")" << std::endl;
}
void Window::mousePress_callback(int button, int action, int mods) {
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
void Window::mouseScroll_callback(double xoffset, double yoffset) {
	return;
}

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