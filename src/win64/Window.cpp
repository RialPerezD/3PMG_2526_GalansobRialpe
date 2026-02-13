
#include "Motarda/win64/Window.hpp"
#include <memory>
#include <vector>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h" 


#include <iostream>

#pragma warning(push)
#pragma warning(disable : 4005)
//Need this include to use WinMain
#include <windows.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#pragma warning(pop)


#include <algorithm>
#include <MotArda/common/Engine.hpp>


static GLenum glCheckError_(const char* file, int line){
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

    struct Window::Data {
        GLFWwindow* glfw_window;
        //< Horizontal size of the window
        int windowWidth_;
        //< Vertical size of the window
        int windowHeight_;
        //< Boolean that activates the debug mode
        bool debug_;
        //< Last frame time
        double lastFrameTime_ = 0.0;
    };


    Window::~Window() {
        if (data) {
            if (data->glfw_window) {
                glfwDestroyWindow(data->glfw_window);
            }
        }
    }


    // Must be defined here as Data is not defined in the header
    Window::Window(Window&& right) = default;
    Window& Window::operator=(Window&& right) = default;


    void Window::checkErrors() {
        if (data->debug_) {
            glCheckError();
        }
    }


    Window::Window(std::unique_ptr<Data> newData,bool debug) :
        data(std::move(newData))
    {
        glfwMakeContextCurrent(data->glfw_window);
        gladLoadGL();

        if (data->debug_)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }


    std::optional<Window> Window::windowCreate(int width, int height, const char* windowName) {

        auto d = std::make_unique<Data>();


        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        d->glfw_window = glfwCreateWindow(width, height, windowName, NULL, NULL);

        if (d->glfw_window == nullptr) {
            return std::nullopt;
        }
        // TODO pasar debug desde parametros de windowCreate
        std::optional<Window> wind = std::make_optional(Window{ std::move(d) ,true });
        wind.value().data->windowWidth_ = width;
        wind.value().data->windowHeight_ = height;
        wind.value().data->debug_ = false;

        return wind;
    }


    bool Window::shouldClose() {
        return glfwWindowShouldClose(data->glfw_window);
    }


    void Window::setDebugMode(bool b) {
        data->debug_ = b;
    }


    void Window::pollEvents() {
        glfwPollEvents();
    }


    void Window::createContext() {

        
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
        return data->windowWidth_ / (float)data->windowHeight_;
    }


    void Window::setKeyCallback(void* keyCallback) {
        auto parsed = reinterpret_cast<void(*)(GLFWwindow*, int, int, int, int)>(keyCallback);
        glfwSetKeyCallback(data->glfw_window, parsed);
    }


    float Window::getLastFrameTime() {
        double currentTime = timer();
        float deltaTime = static_cast<float>(currentTime - data->lastFrameTime_);
        data->lastFrameTime_ = currentTime;
        return deltaTime;
    }


    class Shader {
    public:
        static Shader VertexFromFile(std::filesystem::path file) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: no se pudo abrir el archivo " << filename << std::endl;
                return nullptr;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();

            std::string* shaderSource = new std::string(buffer.str());

            return shaderSource->c_str();

            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShader, 1, &text, NULL);
                glCompileShader(vertexShader);

                if (data->debug_) {
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

                if (data->debug_) {
                    glCheckError();
                }
                return Shader{ vertexShader };
        }

        ~Shader() {
            if (id_ != 0) {
                glDestroyLoQueSea(id_);
            }
        }

        Shader(const Shader&) = delete;
        Shader & operator=(const Shader&) = delete;

        Shader(Shader&& other) : id_{ other.id_ } {
            other.id_ = 0;
        }

        Shader& operator=(Shader&&)//TODO;
    private:
        Shader(GLuint id) : id_{ id } {

        }
        GLuint id_;
    };

    //void Window::openglGenerateVertexShaders(const char* text) {
    //    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //    glShaderSource(vertexShader, 1, &text, NULL);
    //    glCompileShader(vertexShader);

    //    if (data->debug_) {
    //        GLint success;
    //        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    //        if (!success) {
    //            char infoLog[512];
    //            glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
    //            fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    //        }
    //        else {
    //            printf("Vertex shader compiled successfully.\n");
    //        }
    //    }

    //    if (data->debug_) {
    //        glCheckError();
    //    }
    //}


    //void Window::openglGenerateFragmentShaders(const char* text) {
    //    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //    glShaderSource(fragmentShader, 1, &text, NULL);
    //    glCompileShader(fragmentShader);

    //    if (data->debug_) {
    //        GLint success;
    //        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    //        if (!success) {
    //            char infoLog[512];
    //            glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
    //            fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    //        }
    //        else {
    //            printf("Frament shader compiled successfully.\n");
    //        }
    //    }

    //    if (data->debug_) {
    //        glCheckError();
    //    }
    //}


    //void Window::openglCreateProgram() {
    //    program = glCreateProgram();
    //    glAttachShader(program, vertexShader);
    //    glAttachShader(program, fragmentShader);
    //    glLinkProgram(program);


    //    if (data->debug_) {
    //        GLint success;
    //        glGetProgramiv(program, GL_LINK_STATUS, &success);
    //        if (!success) {
    //            char infoLog[512];
    //            glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
    //            fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    //        }
    //        else {
    //            printf("Program linked successfully.\n");

    //        }
    //    }

    //    glUseProgram(program);
    //    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);

    //    if (data->debug_) {
    //        glCheckError();
    //    }
    //}


    //void Window::openglSetUniformsLocationsAndAtributtes(
    //    std::vector<Window::UniformAttrib>& uniforms,
    //    const std::vector<VertexAttrib>& attributes
    //){
    //    // Uniform locations
    //    for (auto& u : uniforms)
    //        u.location = glGetUniformLocation(program, u.name);

    //    // Attribute locations
    //    for (auto& attr : attributes) {
    //        GLint loc = glGetAttribLocation(program, attr.name);
    //        if (loc >= 0) {
    //            glEnableVertexAttribArray(loc);
    //            glVertexAttribPointer(loc, attr.size, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)attr.offset);
    //        }
    //    }

    //    if (data->debug_) {
    //        glCheckError();
    //    }
    //}



    void Window::openglViewportAndClear() {
        glViewport(0, 0, data->windowWidth_, data->windowHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (data->debug_) {
            glCheckError();
        }
    }


    void Window::openglSetUniformsValues(const std::vector<Window::UniformAttrib>& uniforms) {
        for (int i = 0; i < uniforms.size(); i++) {
            float f = *(uniforms[i].values);
            switch (uniforms[i].type) {
            case Window::UniformTypes::Vec2:
                glUniform2fv(uniforms[i].location, 1, uniforms[i].values);
                break;
            case Window::UniformTypes::Vec3:
                glUniform3fv(uniforms[i].location, 1, uniforms[i].values);
                break;
            case Window::UniformTypes::Vec4:
                glUniform4fv(uniforms[i].location, 1, uniforms[i].values);
                break;
            case Window::UniformTypes::Mat2:
                glUniformMatrix2fv(uniforms[i].location, 1, GL_FALSE, uniforms[i].values);
                break;
            case Window::UniformTypes::Mat3:
                glUniformMatrix3fv(uniforms[i].location, 1, GL_FALSE, uniforms[i].values);
                break;
            case Window::UniformTypes::Mat4:
                glUniformMatrix4fv(uniforms[i].location, 1, GL_FALSE, uniforms[i].values);
                break;
            default:
                break;
            }

            if (data->debug_) {
                glCheckError();
            }
        }
    }


    //meter esto en un render system
 /*   void Window::openglProgramUniformDraw(RenderComponent& render) {
        glUseProgram(program);
        auto loc = glGetUniformLocation(program, "diffuseTexture");

        for (size_t i = 0; i < render.meshes_->size(); i++) {
            Mesh* mesh = &render.meshes_->at(i);

            if (mesh->materialId_ != -1) {
                Material mat = render.materials_->at(mesh->materialId_);

                if (!mat.loadeable) continue;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mat.diffuseTexID);
                glUniform1i(loc, 0);

                if (data->debug_) {
                    glCheckError();
                }
            }

            glBindVertexArray(mesh->vao);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->meshSize));

            if (data->debug_) {
                glCheckError();
            }
        }
    }*/


    void Window::openglLoadMaterials(std::vector<Material>& materials) {
        std::unordered_map<std::string, GLuint> textureCache;

        for (auto& mat : materials) {
            if (!mat.diffuseTexPath.empty()) {
                
                std::string route = mat.diffuseTexPath;

                std::string key(route);
                if (textureCache.find(key) != textureCache.end()) {
                    mat.diffuseTexID = textureCache[key];
                    continue;
                }

                GLuint tex = -1;
                glGenTextures(1, &tex);
                glBindTexture(GL_TEXTURE_2D, tex);

                // Configuracion basica de la textura
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                int width, height, channels;
                stbi_set_flip_vertically_on_load(true); // Invierte verticalmente para OpenGL
                unsigned char* data = stbi_load(route.c_str(), &width, &height, &channels, 0);

                if (!data) {
                    std::cerr << "Error cargando textura: " << route << std::endl;
                    mat.diffuseTexID = -1; // -1 significa sin textura
                    continue;
                }

                GLenum format;

                switch (channels) {
                case 1:
                    format = GL_RED;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
                default:
                    format = GL_RGB;
                    break;
                }

                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(data);

                textureCache[key] = tex; // Guardamos la textura en la cache
                mat.diffuseTexID = tex;
                continue;
            }
        }
    }

    Window::ObjItem::ObjItem() : meshes(), materials() {}
}


// Entry point for window:
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
