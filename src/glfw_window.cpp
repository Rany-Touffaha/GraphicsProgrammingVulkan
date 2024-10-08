#include <glfw_window.h>
#include <glfw_monitor.h>
#include <GLFW/glfw3.h>
#include <precomp.h>

namespace veng{

    Window::Window(gsl::czstring name, glm::ivec2 size) {

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(size.x, size.y, name, nullptr, nullptr);
        if(window == nullptr)
            std::exit(EXIT_FAILURE);
    }

    Window::~Window() {
        glfwDestroyWindow(window);
    }

    glm::ivec2 Window::GetWindowSize() const {
        glm::ivec2 windowSize;
        glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
        return windowSize;
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(window);
    }

    GLFWwindow *Window::GetHandle() const {
        return window;
    }

    bool Window::TryMoveToMonitor(std::uint16_t monitorNumber) {
        gsl::span<GLFWmonitor*> monitors = veng::GetMonitors();
        if (monitorNumber < monitors.size()) {
            veng::MoveWindowToMonitor(window, monitors[monitorNumber]);
            return true;
        }

        return false;
    }

    glm::ivec2 Window::GetFrameBufferSize() const {
        glm::ivec2 frameBufferSize;
        glfwGetFramebufferSize(window, &frameBufferSize.x, &frameBufferSize.y);
        return frameBufferSize;
    }
}