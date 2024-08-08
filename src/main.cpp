#include <GLFW/glfw3.h>
#include <glfw_initialisation.h>

using namespace std;
using namespace gsl;
using namespace veng;

int32_t main(int32_t argc, zstring* argv) {

    const GlfwInitialisation glfw;
    not_null window = glfwCreateWindow(800, 600, "Vulkan Engine", nullptr, nullptr);
    final_action cleanupWindow([window] { glfwDestroyWindow(window); });

    int32_t monitorCount = 0;
    GLFWmonitor** monitorPointers = glfwGetMonitors(&monitorCount);
    gsl::span<GLFWmonitor*> monitors(monitorPointers, monitorCount);

    glm::ivec2 monitorPosition;
    glfwGetMonitorPos(monitors[1], &monitorPosition.x, &monitorPosition.y);

    glm::ivec2 windowSize;
    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

    glm::ivec2 monitorSize;
    glfwGetMonitorWorkarea(monitors[1], nullptr, nullptr,&monitorSize.x, &monitorSize.y);

    glm::ivec2 windowNewPosition;
    windowNewPosition = monitorPosition + (monitorSize/2) - (windowSize/2);
    glfwSetWindowPos(window, windowNewPosition.x, windowNewPosition.y);


    while (!glfwWindowShouldClose(window))
        glfwPollEvents();

    return EXIT_SUCCESS;
}
