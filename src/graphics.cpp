#include <precomp.h>
#include <graphics.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#pragma region VK_FUNCTION_EXT_IMPL

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
        VkInstance vkInstance,
        const VkDebugUtilsMessengerCreateInfoEXT* info,
        const VkAllocationCallbacks* allocator,
        VkDebugUtilsMessengerEXT* debugMessenger
        ) {
            PFN_vkCreateDebugUtilsMessengerEXT function =
                    reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT"));

            if (function != nullptr){
                return function(vkInstance, info, allocator, debugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
        VkInstance vkInstance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* allocator
        ) {
            PFN_vkDestroyDebugUtilsMessengerEXT function =
            reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT"));

    if (function != nullptr){
        return function(vkInstance, debugMessenger, allocator);
    }
}

#pragma endregion

namespace veng {

#pragma region VALIDATION_LAYERS

    static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
            void* userData
            ){
        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            spdlog::warn("Vulkan Validation: {}", callbackData->pMessage);
        } else {
            spdlog::error("Vulkan Error: {}", callbackData->pMessage);
        }
        return VK_FALSE;
    }

    static VkDebugUtilsMessengerCreateInfoEXT GetCreateMessengerInfo() {
        VkDebugUtilsMessengerCreateInfoEXT creationInfo = {};
        creationInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        creationInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        creationInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

        creationInfo.pfnUserCallback = ValidationCallback;
        creationInfo.pUserData = nullptr;

        return creationInfo;
    }

    std::vector<VkLayerProperties> Graphics::GetSupportedValidationLayers() {
        std::uint32_t count;
        vkEnumerateInstanceLayerProperties(&count, nullptr);

        if (count==0) return {};

        std::vector<VkLayerProperties> properties(count);
        vkEnumerateInstanceLayerProperties(&count, properties.data());

        return properties;
    }

    bool LayerMatchesName(gsl::czstring name, const VkLayerProperties& layerProperties) {
        return veng::streq(layerProperties.layerName, name);
    }

    bool IsLayerSupported(gsl::span<VkLayerProperties> layers, gsl::czstring name) {
        return std::any_of(layers.begin(), layers.end(),
                           std::bind_front(LayerMatchesName, name));
    }

    bool Graphics::AreAllLayersSupported(gsl::span<gsl::czstring> layers) {
        std::vector<VkLayerProperties> supportedLayers = GetSupportedValidationLayers();
        return std::all_of(layers.begin(), layers.end(), std::bind_front(IsLayerSupported, supportedLayers));
    }

    void Graphics::SetupDebugMessenger() {
        if (!validationEnabled) return;
        VkDebugUtilsMessengerCreateInfoEXT info = GetCreateMessengerInfo();
        VkResult result = vkCreateDebugUtilsMessengerEXT(vkInstance, &info, nullptr, &debugMessenger);
        if (result != VK_SUCCESS){
            spdlog::error("Cannot create debug messenger");
            return;
        }
    }

#pragma endregion

#pragma region INSTANCE_AND_EXTENSIONS

    void Graphics::CreateInstance() {
        std::array<gsl::czstring, 1> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        if (!AreAllLayersSupported(validationLayers))
            validationEnabled = false;

        std::vector<gsl::czstring> requiredExtensions = GetRequiredInstanceExtensions();

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pApplicationName = "Graphics programming in Vulkan";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "VEng";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = requiredExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

        VkDebugUtilsMessengerCreateInfoEXT messengerCreationInfo = GetCreateMessengerInfo();

        if (validationEnabled){
            instanceCreateInfo.pNext = &messengerCreationInfo;
            instanceCreateInfo.enabledLayerCount = validationLayers.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.ppEnabledLayerNames = nullptr;
        }

        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);

        if(result != VK_SUCCESS){
            std::cout << static_cast<std::underlying_type<VkResult>::type>(result) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    gsl::span<gsl::czstring> Graphics::GetSuggestedInstanceExtensions() {
        std::uint32_t  glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        m_extensions.clear();
        m_extensions.insert(m_extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

        m_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        return {m_extensions.data(), m_extensions.size()};
    }

    std::vector<gsl::czstring> Graphics::GetRequiredInstanceExtensions() {
        gsl::span<gsl::czstring> suggestedExtensions = GetSuggestedInstanceExtensions();
        std::vector<gsl::czstring> requiredExtensions(suggestedExtensions.size());
        std::copy(suggestedExtensions.begin(), suggestedExtensions.end(), requiredExtensions.begin());

        if(validationEnabled)
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        if (!AreAllExtensionsSupported(suggestedExtensions))
            std::exit(EXIT_FAILURE);

        return requiredExtensions;
    }

    std::vector<VkExtensionProperties> Graphics::GetSupportedInstanceExtensions() {

        std::uint32_t count;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

        if (count==0) return {};

        std::vector<VkExtensionProperties> properties(count);
        vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());

        return properties;
    }

    bool ExtensionMatchesName(gsl::czstring name, const VkExtensionProperties& extensionProperties) {
        return veng::streq(extensionProperties.extensionName, name);
    }

    bool IsExtensionSupported(gsl::span<VkExtensionProperties> extensions, gsl::czstring name) {
        return std::any_of(extensions.begin(), extensions.end(),
                           std::bind_front(ExtensionMatchesName, name));
    }

    bool Graphics::AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions) {
        std::vector<VkExtensionProperties> supportedExtensions = GetSupportedInstanceExtensions();
        return std::all_of(extensions.begin(), extensions.end(), std::bind_front(IsExtensionSupported, supportedExtensions));
    }

#pragma endregion

#pragma region DEVICES_AND_QUEUES

    Graphics::QueueFamilyIndices Graphics::FindQueueFamilies(VkPhysicalDevice device) {

        std::uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

        auto graphicsFamilyIt = std::find_if(families.begin(), families.end(), [](const VkQueueFamilyProperties& properties) {
        return properties.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT);
        });

        QueueFamilyIndices result;
        result.graphicsFamily = graphicsFamilyIt - families.begin();

        for (std::uint32_t i = 0; i < families.size(); ++i) {
            VkBool32 hasPresentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &hasPresentationSupport);
            if (hasPresentationSupport){
                result.presentationFamily = i;
                break;
            }
        }


        return result;
    }

    std::vector<VkExtensionProperties> Graphics::GetDeviceAvailableExtensions(VkPhysicalDevice device){
        std::uint32_t availableExtensionsCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionsCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtensionsCount, availableExtensions.data());

        return availableExtensions;
    }

    bool Graphics::AreAllDeviceExtensionsSupported(VkPhysicalDevice device) {
        std::vector<VkExtensionProperties> availableExtensions = GetDeviceAvailableExtensions(device);

        return std::all_of(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end(),
                           std::bind_front(IsExtensionSupported, availableExtensions));
    }

    bool Graphics::IsDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices families = FindQueueFamilies(device);
        return families.IsValid() && AreAllDeviceExtensionsSupported(device) && GetSwapChainProperties(device).IsValid();
    }

    void Graphics::PickPhysicalDevice() {
        std::vector<VkPhysicalDevice> devices = GetAvailableDevices();

        std::erase_if(devices, std::not_fn(std::bind_front(&Graphics::IsDeviceSuitable, this)));

        if (devices.empty()){
            spdlog::error("No physical devices that match the criteria");
            std::exit(EXIT_FAILURE);
        }

        physicalDevice = devices[0];
    }

    std::vector<VkPhysicalDevice> Graphics::GetAvailableDevices() {
        std::uint32_t deviceCount;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

        if (deviceCount == 0){
            return {};
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

        return devices;
    }

    void Graphics::CreateLogicalDeviceAndQueues() {
        QueueFamilyIndices pickedDeviceFamilies = FindQueueFamilies(physicalDevice);

        if (!pickedDeviceFamilies.IsValid()){
            std::exit(EXIT_FAILURE);
        }

        std::set<std::uint32_t> uniqueQueueFamilies = {
                pickedDeviceFamilies.graphicsFamily.value(),
                pickedDeviceFamilies.presentationFamily.value()};

        std::float_t queuePriority = 1.0f;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (std::uint32_t uniqueQueueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = uniqueQueueFamily;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueInfo);
        }

        VkPhysicalDeviceFeatures requiredFeatures = {};

        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = queueCreateInfos.size();
        deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceInfo.pEnabledFeatures = &requiredFeatures;
        deviceInfo.enabledExtensionCount = requiredDeviceExtensions.size();
        deviceInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
        deviceInfo.enabledLayerCount = 0;

        VkResult result = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &logicalDevice);
        if(result != VK_SUCCESS){
            std::exit(EXIT_FAILURE);
        }

        vkGetDeviceQueue(logicalDevice, pickedDeviceFamilies.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, pickedDeviceFamilies.presentationFamily.value(), 0, &presentQueue);
    }

#pragma endregion

#pragma region PRESENTATION

    void Graphics::CreateSurface() {
        VkResult result = glfwCreateWindowSurface(vkInstance, window->GetHandle(), nullptr, &surface);
        if (result != VK_SUCCESS) {
            std::exit(EXIT_FAILURE);
        }
    }

    Graphics::SwapChainProperties Graphics::GetSwapChainProperties(VkPhysicalDevice device) {
        SwapChainProperties properties;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &properties.capabilities);

        std::uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        properties.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, properties.formats.data());

        std::uint32_t modesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modesCount, nullptr);
        properties.presentModes.resize(modesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modesCount, properties.presentModes.data());

        return properties;
    }

    bool IsRgbaTypeFormat(const VkSurfaceFormatKHR& formatProperties){
        return formatProperties.format == VK_FORMAT_R8G8B8A8_SRGB ||
               formatProperties.format == VK_FORMAT_B8G8R8A8_SRGB;
    }

    bool IsSrgbColorSpace(const VkSurfaceFormatKHR& formatProperties) {
        return formatProperties.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    }

    bool IsCorrectFormat(const VkSurfaceFormatKHR& formatProperties) {
        return IsRgbaTypeFormat(formatProperties) && IsSrgbColorSpace(formatProperties);
    }

    VkSurfaceFormatKHR Graphics::ChooseSwapSurfaceFormat(gsl::span<VkSurfaceFormatKHR> formats){

        if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED){
            return {VkFormat::VK_FORMAT_R8G8B8A8_SRGB, VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR};
        }

        auto it = std::find_if(formats.begin(), formats.end(), IsCorrectFormat);

        if (it != formats.end()){
            return *it;
        }

        return formats[0];
    }

    bool IsMailboxPresentMode(const VkPresentModeKHR& mode) {
        return mode == VK_PRESENT_MODE_MAILBOX_KHR;
    }

    VkPresentModeKHR Graphics::ChooseSwapPresentMode(gsl::span<VkPresentModeKHR> presentMode) {

        if (std::any_of(presentMode.begin(), presentMode.end(), IsMailboxPresentMode)){
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Graphics::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        constexpr std::uint32_t kInvalidSize = std::numeric_limits<std::uint32_t>::max();
        if (capabilities.currentExtent.width != kInvalidSize){
            return capabilities.currentExtent;
        } else {
            glm::ivec2 size = window->GetFrameBufferSize();
            VkExtent2D actualExtent = {
                    static_cast<std::uint32_t>(size.x),
                    static_cast<std::uint32_t>(size.y),
            };

            actualExtent.width = std::clamp(
                    actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);

            actualExtent.height = std::clamp(
                    actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    std::uint32_t Graphics::ChooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities) {
        std::uint32_t imageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < imageCount){
            imageCount = capabilities.maxImageCount;
        }

        return imageCount;
    }

    void Graphics::CreateSwapChain() {
        SwapChainProperties properties = GetSwapChainProperties(physicalDevice);

        surfaceFormat = ChooseSwapSurfaceFormat(properties.formats);
        presentMode = ChooseSwapPresentMode(properties.presentModes);
        extent = ChooseSwapExtent(properties.capabilities);

        std::uint32_t imageCount = ChooseSwapImageCount(properties.capabilities);

        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = surface;
        info.minImageCount = imageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.presentMode = presentMode;
        info.preTransform = properties.capabilities.currentTransform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.clipped = VK_TRUE;
        info.oldSwapchain = VK_NULL_HANDLE;

        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);


        if (indices.graphicsFamily != indices.presentationFamily){
            std::array<std::uint32_t, 2> familyIndices = {
                    indices.graphicsFamily.value(),
                    indices.presentationFamily.value(),
            };

            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = familyIndices.size();
            info.pQueueFamilyIndices = familyIndices.data();
        } else {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        VkResult result = vkCreateSwapchainKHR(logicalDevice, &info, nullptr, &swapChain);
        if (result != VK_SUCCESS){
            std::exit(EXIT_FAILURE);
        }

        std::uint32_t actualImageCount;
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &actualImageCount, nullptr);
        swapChainImages.resize(actualImageCount);
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &actualImageCount, swapChainImages.data());
    }

    void Graphics::CreateImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        auto imageViewIt = swapChainImageViews.begin();
        for (VkImage image : swapChainImages) {
            VkImageViewCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = image;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = surfaceFormat.format;
            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(logicalDevice, &info, nullptr, &*imageViewIt);
            if (result != VK_SUCCESS){
                std::exit(EXIT_FAILURE);
            }
            std::next(imageViewIt);
        }
    }

#pragma endregion

    Graphics::Graphics(gsl::not_null<Window *> window) : window (window){
    #if !defined(NDEBUG)
        validationEnabled = true;
    #endif

        InitaliseVulkan();
    }

    Graphics::~Graphics() {
        if (logicalDevice != VK_NULL_HANDLE) {

            for (VkImageView imageView : swapChainImageViews) {
                vkDestroyImageView(logicalDevice, imageView, nullptr);
            }

            if (swapChain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
            }
            vkDestroyDevice(logicalDevice, nullptr);
        }

        if (vkInstance != VK_NULL_HANDLE) {
            if (surface != VK_NULL_HANDLE) {
                vkDestroySurfaceKHR(vkInstance, surface, nullptr);
            }

            if (debugMessenger != VK_NULL_HANDLE) {
                vkDestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);
            }
            vkDestroyInstance(vkInstance, nullptr);
        }
    }

    void Graphics::InitaliseVulkan() {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDeviceAndQueues();
        CreateSwapChain();
    }
}