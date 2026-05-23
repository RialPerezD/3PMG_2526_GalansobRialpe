#include <MotArda/Systems/ShadowMapSystem.hpp>
#include <MotArda/Debug.hpp>

namespace MTRD {
    ShadowMapSystem::ShadowMapSystem(glm::mat4& model, bool& debug)
        : debug_(debug),
        shadowProgram{
            Shader::VertexFromFile("romfs:/shadersSwitch/shadow_map_vertex.txt", debug),
            Shader::FragmentFromFile("romfs:/shadersSwitch/shadow_map_fragment.txt", debug),
            debug
        },
        omniShadowProgram{
            Shader::VertexFromFile("romfs:/shadersSwitch/omni_shadow_vertex.txt", debug),
            Shader::GeometryFromFile("romfs:/shadersSwitch/omni_shadow_geom.txt", debug),
            Shader::FragmentFromFile("romfs:/shadersSwitch/omni_shadow_frag.txt", debug),
            debug
        } {
        (void)model;

        attributes = {
            { "position", 3, offsetof(Vertex, position), -1},
            { "uv", 2, offsetof(Vertex, uv), -1},
            { "normal", 3, offsetof(Vertex, normal), -1}
        };

        uniforms = {
            {"lightSpaceMatrix", -1, Window::UniformTypes::Mat4, glm::value_ptr(lightSpaceMatrix_)},
            {"model", -1, Window::UniformTypes::Mat4, glm::value_ptr(model)},
        };
    }

    void ShadowMapSystem::RenderShadowMap(ECSManager& ecs, glm::mat4& model) {
        (void)ecs;
        (void)model;
    }

    void ShadowMapSystem::CreateShadowMapResource(GLuint& fbo, GLuint& depthMap) {
        (void)fbo;
        (void)depthMap;
    }

    void ShadowMapSystem::CreateOmniShadowResource(GLuint& fbo, GLuint& cubemap) {
        (void)fbo;
        (void)cubemap;
    }

    void ShadowMapSystem::DrawCall(ECSManager& ecs, glm::mat4& model, Program& currentProg) {
        (void)ecs;
        (void)model;
        (void)currentProg;
    }
}
