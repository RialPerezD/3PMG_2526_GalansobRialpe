#include <Motarda/Texture.hpp>
#include <MotArda/Logger.hpp>
#include <MotArda/Debug.hpp>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <../include/stb_image.h>

namespace MTRD {
    Texture::Texture(const char* route, bool debug_) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true); // Invierte verticalmente para OpenGL
        unsigned char* data = stbi_load(route, &width, &height, &channels, 0);

        if (!data) {
            MTRD::Logger::error("Error cargando textura: {}\n", route);

            id_ = GL_INVALID_INDEX;
            return;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &id_);

        if (debug_) {
            glCheckError();
        }

        // Configuracion basica de la textura
        glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (debug_) {
            glCheckError();
        }

        GLenum format;
        GLenum internalFormat;
        switch (channels) {
        case 1:
            format = GL_RED;
            internalFormat = GL_R8;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB8;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
            break;
        default:
            format = GL_RGB;
            internalFormat = GL_RGB8;
            break;
        }

        GLsizei levels = static_cast<GLsizei>(std::log2(std::max(width, height))) + 1;

        glTextureStorage2D(id_, levels, internalFormat, width, height);
        glTextureSubImage2D(id_, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(id_);

        if (debug_) {
            glCheckError();
        }

        stbi_image_free(data);

        if (debug_) {
            glCheckError();
        }
    }


    Texture::~Texture() {
        if (id_ != GL_INVALID_INDEX) {
            glDeleteTextures(1, &id_);
        }
    }


    Texture::Texture(Texture&& other) noexcept : id_(other.id_) {
        other.id_ = GL_INVALID_INDEX;
    }


    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (id_ != GL_INVALID_INDEX) {
                glDeleteTextures(1, &id_); // Liberamos el recurso actual antes de aceptar el nuevo
            }
            id_ = other.id_;
            other.id_ = GL_INVALID_INDEX;
        }
        return *this;
    }
}