
#include "Motarda/Window.hpp"
#include <memory>
#include <vector>

//Need this include to use WinMain
#include <windows.h>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;


}
namespace MTRD {

    //Add more variables in body on future
    struct Window::Data {
        GLFWwindow* glfw_window;
    };

    Window::~Window() {
        if (data && data->glfw_window) {
            glfwDestroyWindow(data->glfw_window);
        }
    }

    void Window::checkErrors() {
        if (debug_) {
            glCheckError();
        }
    }


    Window::Window(std::unique_ptr<Data> newData) : data(std::move(newData)) {
    }


    std::optional<Window> Window::windowCreate(int width, int height, const char* windowName) {

        auto d = std::make_unique<Data>();


        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        d->glfw_window = glfwCreateWindow(width, height, windowName, NULL, NULL);

        if (d->glfw_window == nullptr) {
            return std::nullopt;
        }

        std::optional<Window> wind = std::make_optional(Window{ std::move(d) });
        wind.value().windowWidth_ = width;
        wind.value().windowHeight_ = height;
        wind.value().debug_ = false;

        return wind;
    }






    bool Window::shouldClose() {
        return glfwWindowShouldClose(data->glfw_window);
    }


    void Window::setDebugMode(bool b) {
        debug_ = b;
    }


    void Window::pollEvents() {
        glfwPollEvents();
    }


    void Window::createContext() {
        glfwMakeContextCurrent(data->glfw_window);
        gladLoadGL();

        if (debug_)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
        
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


    float Window::getSizeRatio() {
        return windowWidth_ / (float)windowHeight_;
    }


    void Window::setKeyCallback(void* keyCallback) {
        auto parsed = reinterpret_cast<void(*)(GLFWwindow*, int, int, int, int)>(keyCallback);
        glfwSetKeyCallback(data->glfw_window, parsed);
    }


    void Window::openglGenerateBuffers(const void* vertex, size_t verticeSize, int numVertex) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, verticeSize * numVertex, vertex, GL_STATIC_DRAW);
        
        checkErrors();
    }


    void Window::openglGenerateVertexShaders(const char* text) {
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &text, NULL);
        glCompileShader(vertexShader);

        if (debug_) {
            GLint success;
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
                fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
            }
            else {
                printf("Vertex shader compiled successfully.\n");
            }
        }

        checkErrors();
    }


    void Window::openglGenerateFragmentShaders(const char* text) {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &text, NULL);
        glCompileShader(fragmentShader);

        if (debug_) {
            GLint success;
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
                fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
            }
            else {
                printf("Frament shader compiled successfully.\n");
            }
        }

        checkErrors();
    }


    void Window::openglCreateProgram() {
        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);


        if (debug_) {
            GLint success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
                fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
            }
            else {
                printf("Program linked successfully.\n");

            }
        }

        glUseProgram(program);

        checkErrors();
    }

    void Window::openglSetAttributesAndUniforms(
        const std::vector<const char*>& uniforms,
        const std::vector<VertexAttrib>& attributes,
        size_t verticeSize
    ){
        // Uniform locations
        uniformLocations.clear();
        for (auto& u : uniforms)
            uniformLocations.push_back(glGetUniformLocation(program, u));

        // Attribute locations
        for (auto& attr : attributes) {
            GLint loc = glGetAttribLocation(program, attr.name);
            if (loc >= 0) {
                glEnableVertexAttribArray(loc);
                glVertexAttribPointer(loc, attr.size, GL_FLOAT, GL_FALSE, verticeSize, (void*)attr.offset);
            }
        }

        checkErrors();
    }



    void Window::openglViewportAndClear() {
        glViewport(0, 0, windowWidth_, windowHeight_);
        glClear(GL_COLOR_BUFFER_BIT);

        checkErrors();

    }


    void Window::openglProgramUniformDraw(const GLfloat* mvp, int ammountPoints) {
        glUseProgram(program);
        glUniformMatrix4fv(uniformLocations[0], 1, GL_FALSE, mvp);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, ammountPoints);
        
        checkErrors();

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
// int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
int main()
{
    // Initializes the GLFW library and prepares it for use.
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwInit();

    // Calls the main implemented by the usser
    MTRD::main();

    // Ileans up and releases all resources used by the GLFW library.
    glfwTerminate();

    return 0;
}
