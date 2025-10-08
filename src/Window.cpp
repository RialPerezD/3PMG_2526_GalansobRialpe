
#include "Motarda/Window.hpp"
#include <memory>

//Need this include to use WinMain
#include <windows.h>
#include "GLFW/glfw3.h"
#include "gl/glew.h"


namespace MTRD {

    //Add more variables in body on future
    struct Window::Data {
        GLFWwindow* glfw_window;
    };


    Window::Window(Data* newData) {
        data = newData;
    }


    std::optional<Window> Window::windowCreate(int width, int height, const char* windowName) {

        //hacerlo unique pointer
        //pimpl
        Data* data = new Data;

        data->glfw_window = glfwCreateWindow(width, height, windowName, NULL, NULL);
        if (data->glfw_window == nullptr) {
            return std::nullopt;
        }

        std::optional<Window> wind = std::make_optional(Window{ std::move(data) });
        wind.value().windowWidth_ = width;
        wind.value().windowHeight_ = height;

        return wind;
    }


    Window::~Window() {
        if (data) {
            glfwDestroyWindow(data->glfw_window);
            glfwTerminate();
        }
    }


    Window::Window(Window&& right) : data{ right.data } {
        right.data = nullptr;
    }


    bool Window::shouldClose() {
        return glfwWindowShouldClose(data->glfw_window);
    }


    void Window::pollEvents() {
        glfwPollEvents();
    }


    void Window::createContext() {
        glfwMakeContextCurrent(data->glfw_window);
        //gladLoadGL(glfwGetProcAddress);
    }


    void Window::render() {
        glfwGetFramebufferSize(data->glfw_window, &windowWidth_, &windowHeight_);
        glViewport(0, 0, windowWidth_, windowHeight_);
    }


    double Window::timer() {
        return glfwGetTime();
    }


    void Window::swapBuffers() {
        glfwSwapBuffers(data->glfw_window);
    }


    void Window::setSwapInterval(int i) {
        glfwSwapInterval(i);
    }


    void Window::setErrorCallback(void(*function)(int, const char*)) {
        glfwSetErrorCallback(function);
    }


    void Window::openglGenerateBuffers(const void* vertex) {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    }


    void Window::openglGenerateVertexShaders(const char* text) {
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &text, NULL);
        glCompileShader(vertexShader);
    }


    void Window::openglGenerateFragmentShaders(const char* text) {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &text, NULL);
        glCompileShader(fragmentShader);
    }


    void Window::openglCreateProgram() {
        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
    }


    void Window::openglSet3AtribLocations(
        const char* at1,
        const char* at2,
        const char* at3) {
        mvpLocation = glGetUniformLocation(program, at1);
        vposLocation = glGetAttribLocation(program, at2);
        vcolLocation = glGetAttribLocation(program, at3);
    }


    void Window::openglVertexConfig(double size) {
        glEnableVertexAttribArray(vposLocation);
        glVertexAttribPointer(vposLocation, 2, GL_FLOAT,
            GL_FALSE, size, (void*)0);
        glEnableVertexAttribArray(vcolLocation);
        glVertexAttribPointer(vcolLocation, 3, GL_FLOAT,
            GL_FALSE, size, (void*)(sizeof(float) * 2));
    }


    void Window::openglViewportAndClear() {
        glViewport(0, 0, windowWidth_, windowHeight_);
        glClear(GL_COLOR_BUFFER_BIT);
    }


    void Window::openglProgramUniformDraw(const GLfloat* mvp) {
        glUseProgram(program);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }


    float Window::getSizeRatio() {
        return windowWidth_ / (float)windowHeight_;
    }
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
    //dejalo fuera del espacio de nombres aun q se llame mtrdmain
    MTRD::main();

    // Ileans up and releases all resources used by the GLFW library.
    glfwTerminate();

    return 0;
}
