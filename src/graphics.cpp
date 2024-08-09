#include <precomp.h>
#include <graphics.h>
#include <GLFW/glfw3.h>

namespace veng {

    Graphics::Graphics(gsl::not_null<Window *> window) : window(window){
        InitaliseVulkan();
    }

    Graphics::~Graphics() {
        if (vkInstance != nullptr)
            vkDestroyInstance(vkInstance, nullptr);
    }

    void Graphics::InitaliseVulkan() {
        CreateInstance();
    }

    void Graphics::CreateInstance() {

        gsl::span<gsl::czstring> suggestedExtensions = GetSuggestedExtensions();

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
        instanceCreateInfo.enabledExtensionCount = suggestedExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = suggestedExtensions.data();
        instanceCreateInfo.enabledLayerCount = 0;

        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);

        if(result != VK_SUCCESS){
            std::cout << static_cast<std::underlying_type<VkResult>::type>(result) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    gsl::span<gsl::czstring> Graphics::GetSuggestedExtensions() {
        std::uint32_t  glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        m_extensions.clear();
        m_extensions.insert(m_extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

        m_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        return {m_extensions.data(), m_extensions.size()};
    }
}