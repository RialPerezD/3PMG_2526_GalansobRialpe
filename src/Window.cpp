
#include "Motarda/Window.hpp"
#include <memory>

//Need this include to use WinMain
#include <windows.h>
#include "GLFW/glfw3.h"

struct Window::Data {
    GLFWwindow * glfw_window;
};

Window::Window(int width, int height, const char* windowName) {
    //TODO
    //Preguntar esto a Arnau
    data = new Data;
    data->glfw_window = glfwCreateWindow(width, height, windowName, NULL, NULL);
}

Window::~Window() {
    glfwDestroyWindow(data->glfw_window);
}

bool Window::shouldWindowClose() {
    return glfwWindowShouldClose(data->glfw_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}


/* Entry point for window, parameters are:
* 
* -hInstance: handle (ID) for a program when it runs. The operating system uses it to know which EXE is in memory. Some Windows functions need it, like when loading icons or images.
* -hPrevInstance: has no meaning. It was used in 16-bit Windows, but is now always zero.
* -pCmdLine: contains the command-line arguments as a Unicode string.
* -nCmdShow: a flag that indicates whether the main application window is minimized, maximized, or shown normally.
* 
* The function returns an int value. The operating system doesn't use the return value, but you can use the value to pass a status code to another program.
*/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
    // Initializes the GLFW library and prepares it for use.
    glfwInit();

    // Calls the main implemented by the usser
    main();

    // Ileans up and releases all resources used by the GLFW library.
    glfwTerminate();

    return 0;
}

