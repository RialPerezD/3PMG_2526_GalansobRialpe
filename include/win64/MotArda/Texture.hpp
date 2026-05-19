#pragma once

#include <glad/glad.h>

namespace MTRD {
    class Texture {
    private:
        GLuint id_ = GL_INVALID_INDEX;

    public:
        friend class Terrain;

        Texture();
        Texture(const char* route, bool debug = false);

        ~Texture();

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        GLuint getId() const { return id_; }
    };
}