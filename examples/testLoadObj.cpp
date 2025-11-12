#include "MotArda/Engine.hpp"
#include "MotArda/ObjLoader.hpp"

#include <memory>
#include "../deps/glm-master/glm/glm.hpp"
#include "../deps/glm-master/glm/gtc/matrix_transform.hpp"
#include "../deps/glm-master/glm/gtc/type_ptr.hpp"


// Error callback from engine fail...
static void error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw error: %s\n", description);
}


int MTRD::main() {

    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda OBJ Viewer");
    if (!maybeEng.has_value()) return 1;

    auto& eng = maybeEng.value();

    eng.windowSetDebugMode(true);

    eng.windowSetDebugMode(true);
    eng.windowSetErrorCallback(error_callback);
    eng.windowCreateContext();
    eng.windowSetSwapInterval();

    // --- Cargar objs ---
    std::vector <const char*> objsRoutes = {
        "12140_Skull_v3_L2.obj"
    };

    std::atomic<bool> objsLoaded = false;
    std::vector<MTRD::Window::ObjItem> objItemList;

    eng.enqueueTask([&]() {
        objItemList = eng.loadObjs(objsRoutes);
        objsLoaded = true;
        }
    );

    while (!objsLoaded) {
        eng.windowInitFrame();
        printf("Cargando maya...\n");
        eng.windowEndFrame();
    }

    eng.windowLoadAllMaterials(objItemList);

    if (objItemList.size() == 0) return 1;
    const void* vertexBuffer = static_cast<const void*>(objItemList[0].vertex.data());

    // --- Vectores de uniforms y atributos ---
    std::vector<Window::UniformAttrib> uniforms = {
        {"MVP", -1, Window::UniformTypes::Mat4, nullptr},
        {"model", -1, Window::UniformTypes::Mat4, nullptr},
        {"DIFFUSE", -1, Window::UniformTypes::Vec3, nullptr},
        {"SPECULAR", -1, Window::UniformTypes::Vec3, nullptr},
        {"AMBIENT", -1, Window::UniformTypes::Vec3, nullptr}
    };

    std::vector<Window::VertexAttrib> attributes = {
        { "position", 3, offsetof(Vertex, position) }, // glm::vec3 -> 3 floats
        { "uv", 2, offsetof(Vertex, uv) },             // glm::vec2 -> 2 floats
        { "normal", 3, offsetof(Vertex, normal) }      // glm::vec3 -> 3 floats
    };

    const char* vertex_shader = eng.loadShaderFile("../assets/shaders/textured_obj_vertex.txt");
    const char* fragment_shader = eng.loadShaderFile("../assets/shaders/textured_obj_fragment.txt");

    eng.windowOpenglSetup(
        objItemList,
        vertex_shader,
        fragment_shader,
        uniforms,
        attributes
    );

    glm::mat4x4 v, p, mvp, model;

    uniforms[0].values = glm::value_ptr(mvp);
    uniforms[1].values = glm::value_ptr(model);
    uniforms[2].values = glm::value_ptr(objItemList[0].materials[0].diffuse);
    uniforms[3].values = glm::value_ptr(objItemList[0].materials[0].specular);
    uniforms[4].values = glm::value_ptr(objItemList[0].materials[0].ambient);

    float ratio = eng.windowGetSizeRatio();

    float movSpeed = 0.01f;
    float xPos = 0, yPos = -1;
    float rotSpeed = 0.01f, rotationAngle = 0.f;
    float scaSpeed = 0.01f, scale = 0.05f;

    // Camara
    glm::vec3 camPos = glm::vec3(0.f, 0.f, 5.f); // posicion inicial
    glm::vec3 camTarget = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 camUp = glm::vec3(0.f, 1.f, 0.f);

    // Proyeccion
    float fov = glm::radians(45.f);
    float near = 0.1f;
    float far = 100.f;

    while (!eng.windowShouldClose()) {

        eng.windowInitFrame();

        // --- Input del modelo ---
        if (eng.inputIsKeyPressed(Input::Keyboard::D)) xPos += movSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::A)) xPos -= movSpeed;

        if (eng.inputIsKeyPressed(Input::Keyboard::S)) yPos -= movSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::W)) yPos += movSpeed;

        if (eng.inputIsKeyPressed(Input::Keyboard::Q)) rotationAngle -= rotSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::E)) rotationAngle += rotSpeed;

        if (eng.inputIsKeyPressed(Input::Keyboard::Z)) scale -= scaSpeed;
        else if (eng.inputIsKeyPressed(Input::Keyboard::X)) scale += scaSpeed;

        // --- Modelo ---
        model = glm::mat4(1.f);
        model = glm::translate(model, { xPos, yPos, 0.f });
        model = glm::rotate(model, rotationAngle, glm::vec3(0.f, 1.f, 0.f));
        model = glm::rotate(model, -90.f, glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, { scale, scale, scale });

        v = glm::lookAt(camPos, camTarget, camUp);
        p = glm::perspective(fov, ratio, near, far);
        mvp = p * v * model;

        // Dibujar objetos
        eng.windowOpenglSetUniformsValues(uniforms);
        eng.windowOpenglProgramUniformDraw(objItemList);

        eng.windowEndFrame();
    }

    return 0;
}