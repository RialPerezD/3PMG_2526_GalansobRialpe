#pragma once
#include <glad/glad.h>

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

namespace MTRD {
	GLenum glCheckError_(const char* file, int line);
    void APIENTRY glDebugOutput(GLenum source,
        GLenum type,
        unsigned int id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam);
}

