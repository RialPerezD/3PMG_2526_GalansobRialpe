#include <MotArda/Debug.hpp>
#include <cstdio>
#include <string>


namespace MTRD {
    GLenum glCheckError_(const char* file, int line) {
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
            printf("GL ERROR: %s | %s | (%d)\n", error.c_str(), file, line);
        }
        return errorCode;
    }


    void APIENTRY glDebugOutput(GLenum source,
        GLenum type,
        unsigned int id,
        GLenum severity,
        [[maybe_unused]] GLsizei length,
        const char* message,
        [[maybe_unused]] const void* userParam)
    {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::string out = "---------------\n";
        out += "Debug message (" + std::to_string(id) + "): " + message + "\n";

        switch (source) 
        {
            case GL_DEBUG_SOURCE_API:              out += "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:    out += "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:  out += "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:      out += "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:      out += "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:            out += "Source: Other"; break;
        } out += "\n";

        switch (type) {
            case GL_DEBUG_TYPE_ERROR:               out += "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: out += "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  out += "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         out += "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         out += "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              out += "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          out += "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           out += "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               out += "Type: Other"; break;
        } out += "\n";

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:         out += "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       out += "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW:          out += "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: out += "Severity: notification"; break;
        }

        printf("%s\n", out.c_str());
    }
}
