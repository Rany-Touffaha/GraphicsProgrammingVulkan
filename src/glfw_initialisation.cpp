#include <glfw_initialisation.h>
#include <GLFW/glfw3.h>
#include <precomp.h>


veng::GlfwInitialisation::GlfwInitialisation() {
    if(glfwInit() != GLFW_TRUE) {
        exit(EXIT_FAILURE);
    }
}

veng::GlfwInitialisation::~GlfwInitialisation() = default;

