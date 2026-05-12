#pragma once

#include <MotArda/window.hpp>
#include <MotArda/Input.hpp>
#include <MotArda/JobSystem.hpp>
#include <MotArda/ObjLoader.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Sprite.hpp>

#include <MotArda/Systems/RenderLightsSystem.hpp>
#include <MotArda/Systems/ShadowMapSystem.hpp>
#include <MotArda/Systems/RenderSystem.hpp>
#include <MotArda/Systems/RenderDefferredSystem.hpp>
#include <MotArda/Systems/RenderPbrSystem.hpp>

#include <optional>

#include "PhysxMaster.hpp"
#include "MotArda/Terrain.hpp"

namespace MTRD {

    /**
    * @class MotardaEng
    * @brief This class contains the functions to manage the engine related
    * parts, as the window or inputs.
    */
    class MotardaEng {

    public:
        // --- Constructors and operators ---
        /**
        * @brief Destructor
        * @details Destructor of the engine class.
        * It is setted as defatult
        */
        ~MotardaEng() = default;

        /**
        * @brief Copy constructor
        * @param const MotardaEng& right Reference to an Engine type object
        * @details Copy contructor has been removed.
        */
        MotardaEng(const MotardaEng& right) = delete;
        /**
        * @brief Copy assignment
        * @param const MotardaEng& right Reference to an Engine type object
        * @details Copy assignment has been removed.
        */
        MotardaEng& operator=(const MotardaEng& right) = delete;

        /**
        * @brief Move constructor
        * @param const MotardaEng&& right Reference to an Engine type object
        * @details Move constructor is setted as default.
        */
        MotardaEng(MotardaEng&& right) = default;
        /**
        * @brief Move assignment
        * @param const MotardaEng&& right Reference to an Engine type object
        * @details Move assignment is setted as default.
        */
        MotardaEng& operator=(MotardaEng&& right) = default;

        static std::optional<MotardaEng> createEngine(
            int width = 800,
            int height = 600,
            const char* windowName = "Motarda default name");

        // --- *** ---

        // --- Functions ---
        void SetDebugMode(bool debug);

        /**
        * TODO write online documentation
        *
        * OnlineSystem* ActivateOnlineMode(
        * bool ImServer = true,
        * const char* ip = "127.0.0.0",
        * float port = 1234
        * );*/

        bool windowShouldClose();
        /**
        * @brief windowGetTimer.
        * @return Window time in seconds.
        * @details Calls the "timer" function from the Window class.
        */
        double windowGetTimer();
        /**
        * @brief windowEndFrame.
        * @details Calls the "swapBuffers" and "pollEvents" functions from the Window class.
        */
        void windowEndFrame();
        /**
        * @brief windowSetErrorCallback.
        * @param void(*function)(int, const char*) Function to callback.
        * @details Sets the error callback by calling "setErrorCallback"
        * from the Window class.
        */
        void windowSetErrorCallback(void(*function)(int, const char*));
        /**
        * @brief windowGetSizeRatio.
        * @return Return the size ratio.
        * @details Getter of the size ratio. The function calls "getSizeRatio"
        * from Window class.
        */
        float windowGetSizeRatio();
        /**
        * @brief windowEndFrame.
        * @details Clears the buffer of the window by calling "openglViewportAndClear"
        */
        void windowInitFrame();
        /**
        * @brief windowLoadAllMaterials.
        * @param std::vector<MTRD::Window::ObjItem>& objItemsList List that
        * contains the shapes and materials of the objects.
        * @details Calls windowLoadMaterials() inside a for loop in order
        * to load the materials for all the objects.
        */
        void windowLoadAllMaterials(std::vector<std::shared_ptr<ObjItem>>& objItemsList);
        /**
        * @brief windowGetLastFrameTime.
        * @details Manage the time in seconds by calling "getLastFrameTime" from
        * the window class.
        */
        float windowGetLastFrameTime();

        /**
        * @brief inputIsKeyPressed.
        * @param Input::Keyboard key Pressed input by the user.
        * @details Register when the key is pressed.
        */
        bool inputIsKeyPressed(Input::Keyboard key);
        /**
        * @brief inputIsKeyDown.
        * @param Input::Keyboard key Pressed input by the user.
        * @details Register if the key is being pressed (in case the
        * user holds it).
        */
        bool inputIsKeyDown(Input::Keyboard key);
        /**
        * @brief inputIsKeyUp.
        * @param Input::Keyboard key Pressed input by the user.
        * @details Register the exact moment when the input is released.
        */
        bool inputIsKeyUp(Input::Keyboard key);

        bool inputIsMouseButtonPressed(Input::MouseButton button);
        bool inputIsMouseButtonDown(Input::MouseButton button);
        void inputGetMousePosition(int& x, int& y);

        glm::vec3 raycastFromMouse(float maxDistance);

        /**
        * @brief enqueueTask.
        * @param std::function<void()> task Future function to be processed
        * by workers
        * @details Adds a task to the queue by blocking the mutex and waking up
        * a thread.
        */
        void enqueueTask(std::function<void()> task);

        /**
        * @brief loadObjs.
        * @param std::vector <const char*> routes Path of the OBJs to load.
        * @return Returns a list with the OBJs to load.
        * @details First, a list of objects is created, and then, the routes are loaded.
        * Next, a new ObjItem type varaible is created to store the shapes and materials,
        * and finally, we move "item" into the first list of objects.
        */
        std::vector<std::shared_ptr<ObjItem>> loadObjs(const std::vector <const char*>& routes);
        /**
        * @brief loadObjs.
        * @param const char* filename Name of the shader.
        * @return Returns a pointer containing the shader source code.
        * @details Loads the contents of a shader file and returns it as a string.
        */
        const char* loadShaderFile(const char* filename);

        std::unique_ptr<Mesh> createMesh(const std::vector<Vertex>& vertices, const std::string& name);

        std::shared_ptr<ObjItem> generateCube(float size, int texureId = 0, bool debug = true);
        std::shared_ptr<ObjItem> generatePlane(float width, float height, int texureId = 0, bool debug = true);
        std::shared_ptr<ObjItem> generateSphere(float radius, int segments, int rings, int texureId = 0, bool debug = true);

        std::unique_ptr<Terrain> CreateTerrain(
            float width,
            float depth,
            float maxHeight,
            int seed = -1,
            int textureId = 0,
            bool debug = false);

        // TODO , poner en la documentacion que esto inserta un transform y un render components por defecto
        Sprite& generateSprite(GLuint textureIndex, float size, float deep = 0);
        Sprite& generateSpriteSheet(GLuint textureIndex, float size, int frameWidth, int frameHeight, int columns, int rows, float deep = 0);
        //------------

        // --- Utility functions ---
        Camera& getCamera() noexcept { return camera_; }
        const Camera& getCamera() const noexcept { return camera_; }
        ECSManager& getEcs() noexcept { return ecs_; }
        const ECSManager& getEcs() const noexcept { return ecs_; }
        void createPhysxActor(MTRD::PhysxComponent& p, MTRD::TransformComponent& t);
        void hasPhysx(bool has);

        // --- *** ---

        // --- Render Information ---
        enum RenderType {
            Base,
            Lights,
            LightsWithShadows,
            Bidimensional,
            DeferredWithLights,
            Pbr
        };

        void SetRenderType(RenderType type);

        void RenderScene();

        //------------

    private:
        /**
        * @brief Constructor
        * @param Window window Window class-type object.
        * @param Input input Input class-type object.
        * @param JobSystem js JobSystem class-type object
        * @details Constructor of the engine class.
        */
        MotardaEng(Window window, Input input, JobSystem js);

        // --- 1. Recursos base del sistema (Se inicializan primero) ---
        Window window_;
        Input input_;
        JobSystem jobSystem_;

        // --- 2. Estado basico del motor ---
        bool debug_;
        bool online_;

        // --- 3. Sistemas logicos y ECS ---
        Camera camera_;
        ECSManager ecs_;

        // --- Render Information ---
        glm::mat4x4 vp_;
        glm::mat4x4 model_;

        std::unique_ptr<RenderSystem> renderSystem_;
        std::unique_ptr<RenderLightsSystem> renderLightsSystem_;
        std::unique_ptr<ShadowMapSystem> shadowSystem_;
        std::unique_ptr<RenderDefferredSystem> defferredSystem_;
        std::unique_ptr<RenderPbrSystem> pbrSystem_;
        RenderType actualRenderType_ = RenderType::Base;

        bool initialized2D;
        std::shared_ptr<ObjItem> basePlane_;

        bool hasPhysx_;
        PhysxMaster physx_;
        // --- *** ---

        std::vector<Sprite> sprites_;
    };
}