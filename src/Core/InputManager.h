#pragma once
#include <GLFW/glfw3.h>

class InputManager {
public:
    static void Init(GLFWwindow* window) { s_Window = window; }

    static bool IsKeyDown(int glfwKey) {
        return glfwGetKey(s_Window, glfwKey) == GLFW_PRESS;
    }

    static bool IsKeyUp(int glfwKey) {
        return glfwGetKey(s_Window, glfwKey) == GLFW_RELEASE;
    }

private:
    static GLFWwindow* s_Window;
};