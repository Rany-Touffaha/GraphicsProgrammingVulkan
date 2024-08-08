#include <cstdlib>
#include "glfw_initialisation.h"
#include "GLFW/glfw3.h"

using namespace veng;

veng::GlfwInitialisation::GlfwInitialisation() {
    if(glfwInit() != GLFW_TRUE) {
        exit(EXIT_FAILURE);
    }
}

veng::GlfwInitialisation::~GlfwInitialisation() = default;

