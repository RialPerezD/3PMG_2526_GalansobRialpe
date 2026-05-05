#include "Motarda/win64/Texture.hpp"
#include <string>
#include <MotArda/win64/Debug.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h" 

namespace MTRD {
    Texture::Texture(const char* route, bool debug_) {
        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_);

        if (debug_) {
            glCheckError();
        }

        // Configuracion basica de la textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (debug_) {
            glCheckError();
        }

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true); // Invierte verticalmente para OpenGL
        unsigned char* data = stbi_load(route, &width, &height, &channels, 0);

        if (!data) {
            std::cerr << "Error cargando textura: " << route << std::endl;
            id_ = GL_INVALID_INDEX;
            return;
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