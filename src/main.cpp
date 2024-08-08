#include <cstdlib>
#include <cstdint>
#include <gsl/gsl>
#include <GLFW/glfw3.h>
#include <glfw_initialisation.h>

using namespace std;
using namespace gsl;
using namespace veng;

int32_t main(int32_t argc, zstring* argv) {

    GlfwInitialisation glfw;
    not_null<GLFWwindow*> window = glfwCreateWindow(800, 600, "Vulkan Engine", nullptr, nullptr);
    final_action cleanupWindow([window] { glfwDestroyWindow(window); });

    while (!glfwWindowShouldClose(window))
        glfwPollEvents();

    return EXIT_SUCCESS;
}
