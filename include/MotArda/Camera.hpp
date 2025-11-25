#pragma once
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"

namespace MTRD {

    class Camera {
    public:
        // --- Constructors ---
        Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float fovRadians, float aspect, float nearPlane, float farPlane);
        Camera(float fovRadians, float aspect, float nearPlane, float farPlane);
        ~Camera() = default;

        // --- Updates ---
        void updateView();
        void updateProjection();
        void updateViewProj();
        void updateAll();

        // --- Movement ---
        void moveForward(float delta);
        void moveBackward(float delta);
        void moveRight(float delta);
        void moveLeft(float delta);
        void moveUp(float delta);
        void moveDown(float delta);

        // --- Rotation ---
        void rotate(float deltaYaw, float deltaPitch);

        // --- Getters ---
        const glm::mat4& getView() const noexcept;
        const glm::mat4& getProjection() const noexcept;
        const glm::mat4& getViewProj() const noexcept;

        const glm::vec3& getPosition() const noexcept;
        const glm::vec3& getFront() const noexcept;

        // --- Setters ---
        void setPosition(const glm::vec3& pos) noexcept;
        void setAspect(float aspect) noexcept;

    private:
        // --- Camera properties ---
        glm::vec3 position_;
        glm::vec3 target_;
        glm::vec3 up_;
        glm::vec3 front_;
        glm::vec3 right_;

        // --- Parameters ---
        float yaw_;
        float pitch_;
        float fov_;
        float aspect_;
        float near_;
        float far_;
        float moveSpeed_;
        float sensitivity_;

        // --- Matrix ---
        glm::mat4 view_;
        glm::mat4 projection_;
        glm::mat4 viewProj_;
    };
}
