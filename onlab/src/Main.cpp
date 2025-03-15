#include "Main.h"

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

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
    //glfwSetScrollCallback(window, mouseScroll_callback);

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(1100, 100, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        static double tend = 0;
        const double dt = 0.01;
        double tstart = tend;
        tend = glfwGetTime() / (double) 1000.0;
        for (double t = tstart; t < tend; t += dt) {
            double delta = fmin(dt, tend - t);
        }


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}