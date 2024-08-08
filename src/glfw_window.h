#pragma once

struct GLFWwindow;

namespace veng {

    class Window{
    public:
        Window(gsl::czstring name, glm::ivec2 size);
        ~Window();

        [[nodiscard]] glm::ivec2 GetWindowSize() const;
        [[nodiscard]] bool ShouldClose() const;
        [[nodiscard]] GLFWwindow* GetHandle() const;

        bool TryMoveToMonitor(std::uint16_t monitorNumber);

    private:
        GLFWwindow* window;
    };
}