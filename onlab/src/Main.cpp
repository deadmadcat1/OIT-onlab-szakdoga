#include "Main.h"

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "OIT Onlab", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    //glewExperimental = true;
    glewInit();

    fprintf(stdout, "OpenGL version: %s\n", (const char*)glGetString(GL_VERSION));

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouseMove_callback);
    glfwSetMouseButtonCallback(window, mousePress_callback);
    glfwSetFramebufferSizeCallback(window, resize_callback);
    //glfwSetScrollCallback(window, mouseScroll_callback);

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        static double tend = 0;
        const double dt = 0.01;
        double tstart = tend;
        tend = glfwGetTime() / 1000.0;
        for (double t = tstart; t < tend; t += dt) {
            double delta = fmin(dt, tend - t);
        }


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}