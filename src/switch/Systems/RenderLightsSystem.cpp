#include <MotArda/Systems/RenderLightsSystem.hpp>
#include <MotArda/Debug.hpp>

namespace MTRD {
    RenderLightsSystem::RenderLightsSystem(
        glm::mat4x4& vp,
        glm::mat4x4& model,
        glm::vec3& viewPos,
        bool& debug,
        int windowWidth,
        int windowHeight)
        : debug_(debug),
        viewPos_(viewPos),
        vp_(vp),
        program{
            Shader::VertexFromFile("romfs:/shadersSwitch/textured_lights_obj_vertex.txt", debug),
            Shader::FragmentFromFile("romfs:/shadersSwitch/textured_lights_obj_fragment.txt", debug),
            debug
        },
        windowWidth_(windowWidth),
        windowHeight_(windowHeight) {
        attributes = {
            { "position", 3, offsetof(Vertex, position), -1},
            { "uv", 2, offsetof(Vertex, uv), -1},
            { "normal", 3, offsetof(Vertex, normal), -1}
        };

        uniforms = {
            {"VP", -1, Window::UniformTypes::Mat4, glm::value_ptr(vp)},
            {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
            {"viewPos", -1, Window::UniformTypes::Vec3, glm::value_ptr(viewPos_)},
        };
    }

    void RenderLightsSystem::Render(
        ECSManager& ecs,
        glm::mat4x4& model,
        bool hasShadows) {
        (void)ecs;
        (void)model;
        (void)hasShadows;
    }

    void RenderLightsSystem::SetShadowMap(GLuint depthMap) {
        (void)depthMap;
    }

    void RenderLightsSystem::SetShadowMaps(const std::vector<GLuint>& depthMaps) {
        (void)depthMaps;
    }

    void RenderLightsSystem::SetShadowCubemaps(const std::vector<GLuint>& depthCubemaps) {
        (void)depthCubemaps;
    }

    void RenderLightsSystem::DrawCall(ECSManager& ecs, glm::mat4x4& model, size_t loc, const std::vector<size_t>& renderables, size_t shadowMapIndex, bool isOmni) {
        (void)ecs;
        (void)model;
        (void)loc;
        (void)renderables;
        (void)shadowMapIndex;
        (void)isOmni;
    }
}
