#pragma once
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"

namespace MTRD {

    /**
    * @class Camera
    * @brief Manages the camera of the engine.
    */
    class Camera {
    public:
        /**
        * @brief Constructor
        * @details Initializes all the camera variables
        * @param vec3 pos Final position of the camera
        * @param vec3 target Point where the camera initially looks
        * @param vec3 up Up vector
        * @param float fovRadians Field of view in radians
        * @param float aspect Aspect rato 
        * @param float nearPlane Near clipping plane
        * @param float farPlane Far clipping plane
        */
        Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float fovRadians, float aspect, float nearPlane, float farPlane);
        /**
        * @brief Constructor
        * @details Initializes all the camera variables and sets a default
        * value to position_, target_ and up_. It also calls updateAll()
        * @param float fovRadians Field of view in radians
        * @param float aspect Aspect rato
        * @param float nearPlane Near clipping plane
        * @param float farPlane Far clipping plane
        */
        Camera(float fovRadians, float aspect, float nearPlane, float farPlane);
        /**
        * @brief Destructor
        * @details Default destructor
        */
        ~Camera() = default;


        static Camera& CreateCamera(float ratio);

        // --- Updates ---

        /**
        * @brief updateView
        * @details Recalculates the view matrix
        */
        void updateView();
        /**
        * @brief updateProjection
        * @details Creates the perspective projection matrix
        */
        void updateProjection();
        /**
        * @brief updateViewProj
        * @details Combines view and projection
        */
        void updateViewProj();
        /**
        * @brief updateAll
        * @details This function is called whenever a camera
        * parameter changes
        */  
        void updateAll();

        // --- Movement ---

        /**
        * @brief moveForward
        * @details Moves the camera in the direction of front_
        * @param float delta Delta time to make movement consistent
        */
        void moveForward(float delta);
        /**
        * @brief moveBackward
        * @details Moves the camera in the opposite direction
        to front_
        * @param float delta Delta time to make movement consistent
        */
        void moveBackward(float delta);
        /**
        * @brief moveRight
        * @details Moves the camera in the right_ direction
        * @param float delta Delta time to make movement consistent
        */
        void moveRight(float delta);
        /**
        * @brief moveLeft
        * @details Moves the camera in the opposite direction
        to right_ 
        * @param float delta Delta time to make movement consistent
        */
        void moveLeft(float delta);
        /**
        * @brief moveUp
        * @details Moves the camera in up_ direction
        * @param float delta Delta time to make movement consistent
        */
        void moveUp(float delta);
        /**
        * @brief moveDown
        * @details Moves the camera in the opposite direction
        to up_
        * @param float delta Delta time to make movement consistent
        */
        void moveDown(float delta);

        // --- Rotation ---

        /**
        * @brief rotate
        * @details Changes orientation
        * @param float deltaYaw Controls the rotation of Y axis
        * @param float deltaPitch Controls the rotation of X axis
        */
        void rotate(float deltaYaw, float deltaPitch);

        // --- Getters ---

        /**
        * @brief getView
        * @details Gets view matrix
        */
        const glm::mat4& getView() const noexcept;
        /**
        * @brief getProjection
        * @details Gets projection matrix
        */
        const glm::mat4& getProjection() const noexcept;
        /**
        * @brief getViewProj
        * @details Gets projection x view
        */
        const glm::mat4& getViewProj() const noexcept;
        /**
        * @brief getPosition
        * @details Gets camera position
        */
        const glm::vec3& getPosition() const noexcept;
        /**
        * @brief getFront
        * @details Gets the direction the camera is facing
        */
        const glm::vec3& getFront() const noexcept;

        // --- Setters ---

        /**
        * @brief setPosition
        * @details Changes the position and recalculates matrices
        * @param glm::vec3 pos Vector that contains the camera 
        * position
        */
        void setPosition(const glm::vec3& pos) noexcept;
        void setTarget(const glm::vec3& pos) noexcept;
        /**
        * @brief setAspect
        * @details Updates the aspect ratio and recalculates
        the projection
        * @param float aspect
        */
        void setAspect(float aspect) noexcept;

    private:
        // --- Camera properties ---

        //< Camera position
        glm::vec3 position_;
        //< Position where the camera is lookign at
        glm::vec3 target_;
        //< Direction to go up and down
        glm::vec3 up_;
        //< Direction to go front and back 
        glm::vec3 front_;
        //< Direction to go right and left
        glm::vec3 right_;

        // --- Parameters ---
        //< Horizontal rotation
        float yaw_;
        //< Vertical rotation
        float pitch_;
        //< Field of view of the camera
        float fov_;
        //< Proportional relationship between a screen's width and height
        float aspect_;
        //< Near plane of the camera
        float near_;
        //< Far plane of the camera
        float far_;
        //< Movement speed of the camera
        float moveSpeed_;
        //< Mouse sensitivity of the camera
        float sensitivity_;

        // --- Matrix ---
        
        //< View matrix of the camera
        glm::mat4 view_;
        //< Projection matrix of the camera
        glm::mat4 projection_;
        //< View matrix of the camera
        glm::mat4 viewProj_;
    };
}
