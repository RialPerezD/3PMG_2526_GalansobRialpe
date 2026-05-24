#include <MotArda/Texture.hpp>
#include <MotArda/Debug.hpp>
#include <cstdio>
#include <string>
#include <cmath>
#include <algorithm>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace MTRD {
    Texture::Texture() {
        id_ = GL_INVALID_INDEX;
    }

    Texture::Texture(const char* route, bool debug_) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true); // Invierte verticalmente para OpenGL
        unsigned char* data = stbi_load(route, &width, &height, &channels, 0);

        if (!data) {
            { std::ofstream file("testeo.txt", std::ios::app); file << "No se pudo abrir el archivo " << route << "\n"; }

            id_ = GL_INVALID_INDEX;
            return;
        }

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

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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