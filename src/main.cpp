#include <GLFW/glfw3.h>
#include <glfw_initialisation.h>
#include <glfw_monitor.h>

int32_t main(int32_t argc, gsl::zstring* argv) {

    const veng::GlfwInitialisation glfw;
    gsl::not_null window = glfwCreateWindow(800, 600, "Vulkan Engine", nullptr, nullptr);
    gsl::final_action cleanupWindow([window] { glfwDestroyWindow(window); });

    gsl::span<GLFWmonitor*> monitors = veng::GetMonitors();
    if (monitors.size() > 1)
        veng::MoveWindowToMonitor(window, monitors[1]);


    while (!glfwWindowShouldClose(window))
        glfwPollEvents();

    return EXIT_SUCCESS;
}
