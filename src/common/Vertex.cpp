#include "../include/MotArda/common/Vertex.hpp"

namespace MTRD {
    Vertex::Vertex() : position(0.0f), uv(0.0f), normal(0.0f) {}

    Vertex::Vertex(
        glm::vec3 pos,
        glm::vec2 uv,
        glm::vec3 norm
    ) : position(pos), uv(uv), normal(norm) {
    }

}