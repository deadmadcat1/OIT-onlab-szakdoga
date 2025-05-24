#include "Window.h"


bool Window::create() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto monitor = glfwGetPrimaryMonitor();
	auto mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	//window = glfwCreateWindow(mode->width, mode->height, "OIT Onlab", monitor, NULL);	//windowed fullscreen
	//window = glfwCreateWindow(windowWidth, windowHeight, "OIT Onlab", monitor, NULL);	//fullscreen
	window = glfwCreateWindow(windowWidth, windowHeight, "OIT Onlab", NULL, NULL);	//windowed mode

	if (!window)
		return false;

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);

	glewInit();

	std::cout << "OpenGL version: " << (const char*)glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouseMove_callback);
	glfwSetMouseButtonCallback(window, mousePress_callback);
	glfwSetFramebufferSizeCallback(window, resize_callback);
	//glfwSetScrollCallback(window, mouseScroll_callback);

	glfwSwapInterval(1);
	scene = std::make_unique<Scene>();

	return (scene && scene->set());
}

void Window::renderLoop() {
	while (!glfwWindowShouldClose(window))
	{
		static float tend = 0.0f;
		const float dt = 0.01f;
		float tstart = tend;
		tend = (float)glfwGetTime() / 1000.0f;
		for (float t = tstart; t < tend; t += dt) {
			float delta = fmin(dt, tend - t);
			scene->animate(delta);
		}

		scene->render(renderingTransparencyMode);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

Window::~Window()
{
	glfwDestroyWindow(window);
}

//callback implementations

void Window::resize_callback(int width, int height) {
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
	scene->notifyResize(width, height);
}
void Window::key_callback(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			std::cout << "Exiting..." << std::endl;
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_1:
			renderingTransparencyMode = TransparencyMethod::alphaBlend;
			std::cout << "Transparency mode: Alpha Blending" << std::endl;
			break;
		case GLFW_KEY_2:
			renderingTransparencyMode = TransparencyMethod::depthPeel;
			std::cout << "Transparency mode: Depth Peeling" << std::endl;
			break;
		case GLFW_KEY_3:
			renderingTransparencyMode = TransparencyMethod::moment;
			std::cout << "Transparency mode: Moment-based" << std::endl;
			break;
		case GLFW_KEY_4:
			renderingTransparencyMode = TransparencyMethod::wavelet;
			std::cout << "Transparency mode: Wavelet" << std::endl;
			break;
		default:
			break;
		}
	}
}
void Window::mouseMove_callback(double xpos, double ypos) {
	double Cx = screen2NDC(xpos, ypos).x;
	double Cy = screen2NDC(xpos, ypos).y;
	Cx -= mousePrevX;
	Cy -= mousePrevY;
	mousePrevX = Cx;
	mousePrevY = Cy;
	std::cout << Cx << ":" << Cy<< std::endl;//TODO: fix this crap
	if (mouseDragState) {
		scene->changeCameraAttitudeNDC(ypos, xpos);
	}
}
void Window::mousePress_callback(int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			mouseDragState = true;
			double oldX, oldY;
			glfwGetCursorPos(window, &oldX, &oldY);
			mousePrevX = screen2NDC(oldX, oldY).x;
			mousePrevY = screen2NDC(oldX, oldY).y;
			break;
		case GLFW_MOUSE_BUTTON_2:
			break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mouseDragState = false;
			break;
		case GLFW_MOUSE_BUTTON_2:
			break;
		}
	}
}
void Window::mouseScroll_callback(double xoffset, double yoffset) {
	return;
}

glm::vec2 Window::screen2NDC(double xpos, double ypos) {
	double Cx = 2.0f * xpos / windowWidth - 1;
	double Cy = 1.0f - 2.0f * ypos / windowHeight;
	return glm::vec2(Cx, Cy);
}