#pragma once
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/GLFW-OpenGL3.hpp>
#include <GLFW/glfw3.h>

class glfw_helper
{
    public:
    glfw_helper()
    {
        glfwInit();

    // The OpenGL renderer backend in TGUI requires at least OpenGL 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Required for macOS
        window = glfwCreateWindow(370, 700, "Proxy Master", NULL, NULL);
        glfwSetWindowSizeLimits(window, 270, 645, GLFW_DONT_CARE, GLFW_DONT_CARE);

        glfwMakeContextCurrent(window);
    }
    ~glfw_helper()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    public:
    GLFWwindow* window;
};