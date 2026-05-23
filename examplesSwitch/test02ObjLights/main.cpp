#include <MotArda/Engine.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Systems/RenderSystem.hpp>

#include <memory>

#include <switch.h>
#include <unistd.h>

#include <fstream>

static int s_nxlinkSock = -1;

extern "C" void userAppInit() {
    if (R_SUCCEEDED(socketInitializeDefault())) {
        s_nxlinkSock = nxlinkStdio();
        if (s_nxlinkSock < 0) {
            socketExit();
        }
    }else{
        printf("fallo");
    }
}

extern "C" void userAppExit() {
    if (s_nxlinkSock >= 0) {
        close(s_nxlinkSock);
        socketExit();
        s_nxlinkSock = -1;
    }
}

int main(int argc, char* argv[])
{
    romfsInit();
    auto maybeEng = MTRD::MotardaEng::createEngine(1280, 720, "Motarda triangle Switch");

    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();
        eng.setDebugMode(true);

        { std::ofstream file("testeo.txt"); file << "inicio main\n"; }

        eng.setRenderType(MTRD::MotardaEng::RenderType::Base);

        MTRD::Camera& camera = eng.getCamera();
        camera.setPosition(glm::vec3(0.f, 0.f, 0.5f));
        camera.setTarget(glm::vec3(0.f, 0.f, 0.f));

        // --- Create drawable triangles ---
        ECSManager& ecs = eng.getEcs();
        ecs.AddComponentType<MTRD::TransformComponent>();
        ecs.AddComponentType<MTRD::RenderComponent>();

        size_t triangle = ecs.AddEntity();

        MTRD::TransformComponent* t = ecs.AddComponent<MTRD::TransformComponent>(triangle);
        t->position = glm::vec3(0.0f);
        t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
        t->angleRotationRadians = -1;
        t->scale = glm::vec3(0.05f);


        //Set the vertex coordinates to create a custom geometry
        std::vector<MTRD::Vertex> vertexList = {
           {
                { 1.0f,  0.0f, 0.f },
                { 0.0f,  0.0f },
                { 1.0f,  0.0f, 0.f }
            },
           {
                { 0.0f,  1.5f, 0.f},
                { 0.0f,  0.0f},
                { 0.0f,  1.0f, 0.f}
            },

            {
                {-1.0f,  0.0f, 0.f},
                { 0.0f,  0.0f},
                { 0.0f,  0.0f, 1.f}
            }
        };

        std::vector<std::shared_ptr<MTRD::ObjItem>> ObjList;
        ObjList.push_back(std::make_shared<MTRD::ObjItem>());

        // Use createMesh(ListOfVertex, "NameOfTheMesh") to create a mesh with custom vertices
        std::unique_ptr<MTRD::Mesh> TriangleMesh = eng.createMesh(vertexList, "triangle");

        // Initialize material's values
        MTRD::Material mat;
        mat.diffuse = glm::vec3(1.0f);
        mat.specular = glm::vec3(1.0f);
        mat.shininess = 32.0f;
        mat.loadeable = true;
        mat.diffuseTexPath = "romfs:/textures/blank/blank.jpg";

        ObjList[0]->materials.push_back(mat);
        ObjList[0]->meshes.push_back(std::move(TriangleMesh));
        ObjList[0]->meshes[0]->materialId_ = 0;

        eng.windowLoadAllMaterials(ObjList);

        MTRD::RenderComponent* r = ecs.AddComponent<MTRD::RenderComponent>(triangle);
        r->objitem_ = ObjList[0];
        // --- *** ---

        MTRD::Input::StickPosition stickpos_;
        float movSpeed = 0.001f;

        auto trans = ecs.GetComponent<MTRD::TransformComponent>(triangle);
        float tolerance = 0.0001f;

        // The main loop will run until the user closes the window
        while (!eng.windowShouldClose()) {
            // Creates a new frame
            eng.windowInitFrame();


            stickpos_ = eng.inputGetLeftStickPosition();
            if (stickpos_.x > tolerance) trans->position.x += movSpeed;
            if (stickpos_.x < -tolerance) trans->position.x -= movSpeed;
            if (stickpos_.y > tolerance) trans->position.y += movSpeed;
            if (stickpos_.y < -tolerance) trans->position.y -= movSpeed;

            eng.renderScene();

            // Ends the frame
            eng.windowEndFrame();
        }

        return 0;
    }

    romfsExit();
    return 1;
}
