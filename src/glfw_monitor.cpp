#include <glfw_monitor.h>
#include "GLFW/glfw3.h"

namespace veng {

    gsl::span<GLFWmonitor*> GetMonitors(){
        int32_t monitorCount = 0;
        GLFWmonitor** monitorPointers = glfwGetMonitors(&monitorCount);
        return {monitorPointers, static_cast<size_t>(monitorCount)};
    }

    glm::ivec2 GetMonitorPosition(gsl::not_null<GLFWmonitor*> monitor){
        glm::ivec2 monitorPosition;
        glfwGetMonitorPos(monitor, &monitorPosition.x, &monitorPosition.y);
        return monitorPosition;
    }

    glm::ivec2 GetMonitorWorkAreaSize(gsl::not_null<GLFWmonitor*> monitor){
        glm::ivec2 monitorSize;
        glfwGetMonitorWorkarea(monitor, nullptr, nullptr,&monitorSize.x, &monitorSize.y);
        return monitorSize;
    }

    void MoveWindowToMonitor(gsl::not_null<GLFWwindow*> window, gsl::not_null<GLFWmonitor*> monitor){
        glm::ivec2 windowSize;
        glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

        const glm::ivec2 windowNewPosition = GetMonitorPosition(monitor) + (GetMonitorWorkAreaSize(monitor)/2) - (windowSize/2);
        glfwSetWindowPos(window, windowNewPosition.x, windowNewPosition.y);
    }
}