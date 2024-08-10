#pragma once

#include <vulkan/vulkan.h>
#include <glfw_window.h>

namespace veng {

    class Graphics final{
    public:
        Graphics(gsl::not_null<Window*> window);
        ~Graphics();
    private:

        struct QueueFamilyIndices {
            std::optional<std::uint32_t> graphicsFamily = std::nullopt;
            std::optional<std::uint32_t> presentationFamily = std::nullopt;

            bool IsValid() const { return graphicsFamily.has_value() /* && presentationFamily.has_value() */ ;}
        };

        void InitaliseVulkan();
        void CreateInstance();
        void SetupDebugMessenger();
        void PickPhysicalDevice();
        void CreateLogicalDeviceAndQueues();
        gsl::span<gsl::czstring> GetSuggestedInstanceExtensions();
        std::vector<gsl::czstring> GetRequiredInstanceExtensions();

        static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();
        static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions);

        static std::vector<VkLayerProperties> GetSupportedValidationLayers();
        static bool AreAllLayersSupported(gsl::span<gsl::czstring> layers);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);
        std::vector<VkPhysicalDevice> GetAvailableDevices();

        VkInstance vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice logicalDevice = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger{};
        gsl::span<gsl::czstring> m_suggestedExtensions;
        std::vector<gsl::czstring> m_extensions;
        gsl::not_null<Window*> window;
        bool validationEnabled = false;

    };
}