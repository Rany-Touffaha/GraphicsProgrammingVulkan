#include <GLFW/glfw3.h>
#include <glfw_initialisation.h>
#include <glfw_window.h>
#include <precomp.h>
#include "graphics.h"


int32_t main(int32_t argc, gsl::zstring* argv) {

    const veng::GlfwInitialisation glfw;

    veng::Window window("Vulkan Engine", {800, 600});

    window.TryMoveToMonitor(1);

    veng::Graphics graphics(&window);

    while (!window.ShouldClose())
        glfwPollEvents();

    return EXIT_SUCCESS;
}
