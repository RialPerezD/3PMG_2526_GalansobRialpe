#pragma once
#include <glad/glad.h>

namespace MTRD {
    class Texture {
    public:
        static GLuint LoadTexture(const char* route, bool debug = false);
    };
}