#include <MotArda/win64/Systems/ShadowMapSystem.hpp>
#include <MotArda/win64/Debug.hpp>

namespace MTRD {
    ShadowMapSystem::ShadowMapSystem()
        : shadowProgram{
            Shader::VertexFromFile("../assets/shaders/shadow_map_vertex.txt", true),
            Shader::FragmentFromFile("../assets/shaders/shadow_map_fragment.txt", true), true }
    {
    }

    ShadowMapSystem::~ShadowMapSystem() {
    }

    void ShadowMapSystem::RenderShadowMap(ECSManager& ecs, std::vector<size_t> renderables, const glm::mat4& lightSpaceMatrix) {
    }
}