#include <glfw_initialisation.h>
#include <GLFW/glfw3.h>
#include <precomp.h>
#include <spdlog/spdlog.h>

namespace veng {

    void glfwErrorCallback(std::int32_t errorCode, gsl::czstring message){
        spdlog::error("Glfw Validation: {}", message);
    }

    veng::GlfwInitialisation::GlfwInitialisation() {
        glfwSetErrorCallback(glfwErrorCallback);

        if (glfwInit() != GLFW_TRUE) {
            exit(EXIT_FAILURE);
        }
    }

    veng::GlfwInitialisation::~GlfwInitialisation() {
        glfwTerminate();
    }

}