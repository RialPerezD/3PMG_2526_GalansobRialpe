#include <MotArda/Camera.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace MTRD {
    Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float fovRadians, float aspect, float nearPlane, float farPlane)
        : position_(pos),
        target_(target),
        up_(up),
        front_(0.0f, 0.0f, -1.0f),
        right_(1.0f, 0.0f, 0.0f),
        yaw_(-90.0f),
        pitch_(0.0f),
        fov_(fovRadians),
        aspect_(aspect),
        near_(nearPlane),
        far_(farPlane),
        moveSpeed_(2.5f),
        sensitivity_(0.1f),
        view_(1.0f),
        projection_(1.0f),
        viewProj_(1.0f)
    {
        updateAll();
    }


    Camera::Camera(float fovRadians, float aspect, float nearPlane, float farPlane)
        : position_(0.0f, 0.0f, 5.0f),
        target_(0.0f, 0.0f, 0.0f),
        up_(0.0f, 1.0f, 0.0f),
        front_(0.0f, 0.0f, -1.0f),
        right_(1.0f, 0.0f, 0.0f),
        yaw_(-90.0f),
        pitch_(0.0f),
        fov_(fovRadians),
        aspect_(aspect),
        near_(nearPlane),
        far_(farPlane),
        moveSpeed_(2.5f),
        sensitivity_(0.1f),
        view_(1.0f),
        projection_(1.0f),
        viewProj_(1.0f)
    {
        updateAll();
    }


    Camera& Camera::CreateCamera(float ratio) {
        static MTRD::Camera camera(
            glm::vec3(0.f, 0.f, 5.f),
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f),
            glm::radians(45.f),
            ratio,
            0.1f,
            1000.f
        );

        camera.updateAll();

        return camera;
    }


    // --- Matrix update ---
    void Camera::updateView() {
        front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_.y = sin(glm::radians(pitch_));
        front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front_);

        glm::vec3 tempUp = up_;
        if (glm::abs(glm::dot(front_, tempUp)) > 0.999f) {
            tempUp = (tempUp.z == 0.0f) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        }

        right_ = glm::normalize(glm::cross(front_, tempUp));
        view_ = glm::lookAt(position_, position_ + front_, glm::cross(right_, front_));
    }


    void Camera::updateProjection() {
        projection_ = glm::perspective(fov_, aspect_, near_, far_);
    }


    void Camera::updateViewProj() {
        viewProj_ = projection_ * view_;
    }


    void Camera::updateAll() {
        updateView();
        updateProjection();
        updateViewProj();
    }


    // --- Movement ---
    //update mover
    void Camera::moveForward(float delta) { position_ += front_ * delta; updateAll(); }
    void Camera::moveBackward(float delta) { position_ -= front_ * delta; updateAll(); }
    void Camera::moveRight(float delta) { position_ += right_ * delta; updateAll(); }
    void Camera::moveLeft(float delta) { position_ -= right_ * delta; updateAll(); }
    void Camera::moveUp(float delta) { position_ += up_ * delta; updateAll(); }
    void Camera::moveDown(float delta) { position_ -= up_ * delta; updateAll(); }


    // --- Rotate ---
    void Camera::rotate(float deltaYaw, float deltaPitch) {
        yaw_ += deltaYaw * sensitivity_;
        pitch_ += deltaPitch * sensitivity_;

        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;

        updateAll();
    }


    // --- Getters ---
    const glm::mat4& Camera::getView() const noexcept { return view_; }
    const glm::mat4& Camera::getProjection() const noexcept { return projection_; }
    const glm::mat4& Camera::getViewProj() const noexcept { return viewProj_; }

    Frustum Camera::getFrustum() const noexcept {
        Frustum frustum;
        const glm::mat4& vp = viewProj_;

        frustum.planes[0].normal = glm::vec3(
            vp[0][3] + vp[0][0],
            vp[1][3] + vp[1][0],
            vp[2][3] + vp[2][0]
        );
        frustum.planes[0].d = vp[3][3] + vp[3][0];

        frustum.planes[1].normal = glm::vec3(
            vp[0][3] - vp[0][0],
            vp[1][3] - vp[1][0],
            vp[2][3] - vp[2][0]
        );
        frustum.planes[1].d = vp[3][3] - vp[3][0];

        frustum.planes[2].normal = glm::vec3(
            vp[0][3] + vp[0][1],
            vp[1][3] + vp[1][1],
            vp[2][3] + vp[2][1]
        );
        frustum.planes[2].d = vp[3][3] + vp[3][1];

        frustum.planes[3].normal = glm::vec3(
            vp[0][3] - vp[0][1],
            vp[1][3] - vp[1][1],
            vp[2][3] - vp[2][1]
        );
        frustum.planes[3].d = vp[3][3] - vp[3][1];

        frustum.planes[4].normal = glm::vec3(
            vp[0][3] + vp[0][2],
            vp[1][3] + vp[1][2],
            vp[2][3] + vp[2][2]
        );
        frustum.planes[4].d = vp[3][3] + vp[3][2];

        frustum.planes[5].normal = glm::vec3(
            vp[0][3] - vp[0][2],
            vp[1][3] - vp[1][2],
            vp[2][3] - vp[2][2]
        );
        frustum.planes[5].d = vp[3][3] - vp[3][2];

        for (int i = 0; i < 6; i++) {
            float length = glm::length(frustum.planes[i].normal);
            if (length > 0.0f) {
                frustum.planes[i].normal /= length;
                frustum.planes[i].d /= length;
            }
        }

        return frustum;
    }

    glm::vec3& Camera::getPosition() noexcept { return position_; }
    const glm::vec3& Camera::getFront() const noexcept { return front_; }


    // --- Setters ---
    void Camera::setPosition(const glm::vec3& pos) noexcept {
        position_ = pos;
        updateAll();
    }

    void Camera::setTarget(const glm::vec3& pos) noexcept {
        target_ = pos;

        glm::vec3 direction = glm::normalize(target_ - position_);

        pitch_ = glm::degrees(asin(direction.y));
        yaw_ = glm::degrees(atan2(direction.z, direction.x));

        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;

        updateAll();
    }

    void Camera::setAspect(float aspect) noexcept {
        aspect_ = aspect;
        updateProjection();
        updateViewProj();
    }

}
