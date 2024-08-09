#pragma once

#include <vulkan/vulkan.h>
#include <glfw_window.h>

namespace veng {

    class Graphics final{
    public:
        Graphics(gsl::not_null<Window*> window);
        ~Graphics();
    private:

        void InitaliseVulkan();
        void CreateInstance();
        gsl::span<gsl::czstring> GetSuggestedExtensions();


        VkInstance vkInstance = VK_NULL_HANDLE;
        gsl::span<gsl::czstring> m_suggestedExtensions;
        std::vector<gsl::czstring> m_extensions;
        gsl::not_null<Window*> window;
    };
}