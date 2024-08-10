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
        void SetupDebugMessenger();
        gsl::span<gsl::czstring> GetSuggestedInstanceExtensions();
        std::vector<gsl::czstring> GetRequiredInstanceExtensions();

        static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
        static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions);

        static std::vector<VkLayerProperties> GetSupportedValidationLayers();
        static bool AreAllLayersSupported(gsl::span<gsl::czstring> layers);


        VkInstance vkInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger;
        gsl::span<gsl::czstring> m_suggestedExtensions;
        std::vector<gsl::czstring> m_extensions;
        gsl::not_null<Window*> window;
        bool validationEnabled = false;
    };
}