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

            bool IsValid() const { return graphicsFamily.has_value() && presentationFamily.has_value();}
        };

        struct SwapChainProperties {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;

            bool IsValid() const { return !formats.empty() && !presentModes.empty();}
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
        SwapChainProperties GetSwapChainProperties(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);
        std::vector<VkPhysicalDevice> GetAvailableDevices();

        void CreateSurface();
        void CreateSwapChain();
        bool AreAllDeviceExtensionsSupported(VkPhysicalDevice device);

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(gsl::span<VkSurfaceFormatKHR> formats);
        VkPresentModeKHR ChooseSwapPresentMode(gsl::span<VkPresentModeKHR> presentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        std::array<gsl::czstring, 1> requiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkInstance vkInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger{};

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice logicalDevice = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        VkSurfaceKHR surface  = VK_NULL_HANDLE;

        gsl::span<gsl::czstring> m_suggestedExtensions;
        std::vector<gsl::czstring> m_extensions;
        gsl::not_null<Window*> window;
        bool validationEnabled = false;

        std::vector<VkExtensionProperties> GetDeviceAvailableExtensions(VkPhysicalDevice device);
    };
}