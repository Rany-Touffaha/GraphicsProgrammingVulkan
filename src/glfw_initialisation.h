#pragma once

namespace veng {

    struct GlfwInitialisation {
    public:
        GlfwInitialisation();
        ~GlfwInitialisation();

        GlfwInitialisation(const GlfwInitialisation&) = delete;
        GlfwInitialisation& operator=(const GlfwInitialisation&) = delete;
    };

}


