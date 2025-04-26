#include "Window.h"

int main(void)
{
    Window window;
    if (!window.create()) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    window.renderLoop();

    glfwTerminate();
    exit(EXIT_SUCCESS);
}