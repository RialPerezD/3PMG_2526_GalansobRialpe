# MotArda Engine - User Manual

---

## Table of Contents

1. [Introduction](#introduction)
2. [System Requirements](#system-requirements)
3. [Building the Project](#building-the-project)
4. [Engine Architecture](#engine-architecture)
5. [Getting Started](#getting-started)
6. [Rendering 3D Objects](#rendering-3d-objects)
7. [Loading OBJ Models](#loading-obj-models)
8. [Camera System](#camera-system)
9. [Entity Component System](#entity-component-system)
10. [Lighting System](#lighting-system)
11. [2D Rendering Mode](#2d-rendering-mode)
12. [Input System](#input-system)
13. [Render Types](#render-types)
14. [Built-in Geometry](#built-in-geometry)
15. [API Reference](#api-reference)
16. [Troubleshooting](#troubleshooting)

---

## 1. Introduction

MotArda is a custom 3D rendering engine built on OpenGL. It provides a simplified interface for creating 3D applications with features including:

- Window management
- 3D rendering with custom meshes
- OBJ model loading
- Multiple light types (Spot, Point, Directional)
- Shadow mapping
- Entity Component System (ECS)
- 2D rendering mode

---

## 2. System Requirements

### Software
- Windows 10/11
- Visual Studio 2019 or later
- Premake (for project generation)
- Conan (for dependency management)

### Hardware
- OpenGL 4.0 compatible graphics card
- Minimum 4GB RAM

---

## 3. Building the Project

### Step 1: Generate Project Files
Run the project generation script:
```bash
./tools/genproyect.bat
```

### Step 2: Open Solution
Open the generated solution file:
```
./build/MotArda.sln
```

### Step 3: Build and Run
Compile the project in Visual Studio and run the executable.

---

## 4. Engine Architecture

The MotArda engine consists of several core components:

| Component | Description |
|----------|-------------|
| `MotardaEng` | Main engine class |
| `ECSManager` | Entity Component System manager |
| `Camera` | Camera handling |
| `RenderSystem` | Rendering pipeline |
| `LightComponent` | Light management |

---

## 5. Getting Started

### Basic Window Creation

```cpp
#include "Motarda/common/Engine.hpp"

int MTRD::main() {
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Window Title");

    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();

        while (!eng.windowShouldClose()) {
            eng.windowInitFrame();
            // Application logic
            eng.windowEndFrame();
        }
        return 0;
    }
    return 1;
}
```

### Standard Application Structure

1. Create engine instance
2. Configure render settings
3. Initialize ECS components
4. Create entities with components
5. Enter main render loop
6. Handle cleanup

---

## 6. Rendering 3D Objects

### Creating Custom Meshes

Define vertices with position, UV coordinates, and normals:

```cpp
std::vector<Vertex> vertexList = {
    {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Position, UV, Normal
    {{0.0f, 1.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};

std::unique_ptr<Mesh> mesh = eng.createMesh(vertexList, "mesh_name");
```

### Defining Materials

```cpp
Material mat;
mat.diffuse = glm::vec3(1.0f, 0.0f, 0.0f);    // RGB color
mat.specular = glm::vec3(1.0f, 1.0f, 1.0f);  // Specular reflection
mat.shininess = 32.0f;                        // Shininess factor
mat.loadeable = true;
mat.diffuseTexPath = "../assets/textures/blank/blank.jpg";
```

### Rendering the Scene

```cpp
std::vector<ObjItem> ObjList;
ObjList.push_back(ObjItem());
ObjList[0].materials.push_back(mat);
ObjList[0].meshes.push_back(std::move(mesh));

eng.windowLoadAllMaterials(ObjList);

RenderComponent* r = ecs.AddComponent<RenderComponent>(entity);
r->meshes_ = &ObjList[0].meshes;
r->materials_ = &ObjList[0].materials;

// In main loop
eng.RenderScene();
```

---

## 7. Loading OBJ Models

The engine supports asynchronous OBJ model loading:

```cpp
std::vector<const char*> objsRoutes = { "model.obj" };
std::atomic<bool> objsLoaded = false;
std::vector<ObjItem> ObjList;

// Load asynchronously
eng.enqueueTask([&]() {
    ObjList = eng.loadObjs(objsRoutes);
    objsLoaded = true;
});

// In main loop - initialize after loading
if (objsLoaded && firstTime) {
    firstTime = false;
    eng.windowLoadAllMaterials(ObjList);
    r->meshes_ = &ObjList[0].meshes;
    r->materials_ = &ObjList[0].materials;
}
```

---

## 8. Camera System

### Initialization

```cpp
MTRD::Camera& camera = eng.getCamera();
camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));  // Camera position
camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));    // Look-at target
```

### Camera Methods

| Method | Description |
|--------|-------------|
| `setPosition(vec3)` | Set camera position |
| `setTarget(vec3)` | Set look-at target |
| `moveForward(float)` | Move forward |
| `moveBackward(float)` | Move backward |
| `moveLeft(float)` | Move left |
| `moveRight(float)` | Move right |
| `moveUp(float)` | Move up |
| `moveDown(float)` | Move down |
| `rotate(float, float)` | Rotate camera (yaw, pitch) |

---

## 9. Entity Component System

### Component Types

| Component | Purpose |
|-----------|---------|
| `TransformComponent` | Position, rotation, scale |
| `RenderComponent` | Mesh and material references |
| `MovementComponent` | Movement parameters |
| `LightComponent` | Light source data |

### Creating Entities

```cpp
ECSManager& ecs = eng.getEcs();

// Register required components
ecs.AddComponentType<TransformComponent>();
ecs.AddComponentType<RenderComponent>();
ecs.AddComponentType<MovementComponent>();

// Create entity
size_t entity = ecs.AddEntity();

// Add TransformComponent
TransformComponent* t = ecs.AddComponent<TransformComponent>(entity);
t->position = glm::vec3(0.0f, 0.0f, 0.0f);
t->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
t->angleRotationRadians = -1.0f;
t->scale = glm::vec3(1.0f);

// Add RenderComponent
RenderComponent* r = ecs.AddComponent<RenderComponent>(entity);
r->meshes_ = &meshList;
r->materials_ = &materialList;

// Add MovementComponent
MovementComponent* m = ecs.AddComponent<MovementComponent>(entity);
m->position = glm::vec3(0.0f);
m->rotation = glm::vec3(0.0f, 0.0f, 1.0f);
m->scale = glm::vec3(0.0f);
m->shouldConstantMove = false;
```

---

## 10. Lighting System

### Light Types

#### Spot Light
```cpp
lightComp->spotLights.push_back(
    MTRD::SpotLight(
        glm::vec3(0.0f, 0.0f, 0.0f),    // Position
        glm::vec3(0.0f, 0.0f, 1.0f),    // Direction
        5.0f,                            // Cutoff angle
        1.0f,                            // Outer cutoff
        0.75f,                           // Exponential decay
        1.0f,                            // Intensity
        0.09f,                           // Linear attenuation
        0.032f,                          // Quadratic attenuation
        aspectRatio
    )
);
```

#### Point Light
```cpp
lightComp->pointLights.push_back(
    MTRD::PointLight(
        glm::vec3(0.0f, 1.0f, 0.0f),  // Position
        glm::vec3(1.0f, 1.0f, 0.0f),   // Color
        1.0f,                          // Intensity
        0.09f,                         // Linear attenuation
        0.032f                         // Quadratic attenuation
    )
);
```

#### Directional Light
```cpp
lightComp->directionalLights.push_back(
    MTRD::DirectionalLight(
        glm::vec3(-1.0f, -1.0f, 0.0f),  // Direction
        glm::vec3(0.0f, 1.0f, 1.0f),    // Color
        0.05f                           // Intensity
    )
);
```

### Adding Light Component

```cpp
ecs.AddComponentType<LightComponent>();
size_t lightEntity = ecs.AddEntity();
LightComponent* lightComp = ecs.AddComponent<LightComponent>(lightEntity);
```

---

## 11. 2D Rendering Mode

Enable 2D rendering:

```cpp
eng.SetRenderType(MotardaEng::RenderType::Bidimensional);

Sprite sprite = eng.generateSprite("texture.png", 1);
```

---

## 12. Input System

### Keyboard Input

```cpp
if (eng.inputIsKeyPressed(Input::Keyboard::W)) {
    camera.moveForward(speed);
}
```

### Available Keys

| Key | Code |
|-----|------|
| W | `Input::Keyboard::W` |
| A | `Input::Keyboard::A` |
| S | `Input::Keyboard::S` |
| D | `Input::Keyboard::D` |
| Q | `Input::Keyboard::Q` |
| E | `Input::Keyboard::E` |
| R | `Input::Keyboard::R` |
| T | `Input::Keyboard::T` |
| F | `Input::Keyboard::F` |
| G | `Input::Keyboard::G` |
| I | `Input::Keyboard::I` |
| J | `Input::Keyboard::J` |
| K | `Input::Keyboard::K` |
| L | `Input::Keyboard::L` |

---

## 13. Render Types

| Type | Description |
|------|-------------|
| `Base` | Basic 3D rendering without lighting |
| `LightsWithShadows` | Full lighting with shadow mapping |
| `Bidimensional` | 2D orthographic rendering |

```cpp
eng.SetRenderType(MotardaEng::RenderType::LightsWithShadows);
```

---

## 14. Built-in Geometry

### Sphere
```cpp
ObjItem sphere = eng.generateSphere(radius, widthSegments, heightSegments, texId);
```

### Plane
```cpp
ObjItem plane = eng.generatePlane(width, height, texId);
```

### Cube
```cpp
ObjItem cube = eng.generateCube(size);
```

### Sprite
```cpp
Sprite sprite = eng.generateSprite(texturePath, zDepth);
```

---

## 15. API Reference

### MotardaEng Methods

| Method | Description |
|--------|-------------|
| `createEngine(width, height, title)` | Create engine instance |
| `getCamera()` | Get camera reference |
| `getEcs()` | Get ECS manager |
| `createMesh(vertices, name)` | Create custom mesh |
| `loadObjs(paths)` | Load OBJ models |
| `generateSphere(...)` | Generate sphere geometry |
| `generatePlane(...)` | Generate plane geometry |
| `generateCube(...)` | Generate cube geometry |
| `generateSprite(...)` | Generate 2D sprite |
| `windowInitFrame()` | Initialize frame |
| `windowEndFrame()` | End frame |
| `windowShouldClose()` | Check if window closed |
| `windowGetSizeRatio()` | Get aspect ratio |
| `RenderScene()` | Render the scene |
| `SetDebugMode(bool)` | Enable/disable debug |
| `SetRenderType(type)` | Set render mode |
| `windowSetErrorCallback(cb)` | Set error callback |

---

## 16. Troubleshooting

### Window Won't Open
- Verify graphics drivers are installed
- Ensure OpenGL 4.0+ support

### Nothing Renders
- Verify `RenderScene()` is called in loop
- Check mesh is attached to RenderComponent
- Confirm camera position and target are set correctly

### Shadows Not Working
- Confirm `RenderType::LightsWithShadows` is set
- Ensure at least one light exists in scene

### OBJ Loading Fails
- Verify file path is correct
- Check file permissions

### Compilation Errors
- Ensure project is regenerated with `genproyect.bat`
- Verify all dependencies are installed via Conan

---

## Appendix: File Structure

```
project/
├── build/                  # Generated solution
├── deps/                   # Dependencies (GLM, etc.)
├── doc/                    # Documentation
├── examples/               # Test/example files
├── assets/
│   ├── models/             # OBJ files
│   ├── shaders/            # GLSL shaders
│   └── textures/           # Image textures
├── src/                    # Source code
└── tools/                  # Build tools
```

---

## Appendix: Example File Reference

| Example File | Demonstrates |
|--------------|---------------|
| `testWindow.cpp` | Basic window creation |
| `testTriangle.cpp` | Custom mesh rendering |
| `testMovTriangle.cpp` | Camera movement |
| `testLoadObj.cpp` | OBJ model loading |
| `testLights.cpp` | Lighting and shadows |
| `testEcs.cpp` | ECS system usage |
| `test2D.cpp` | 2D rendering mode |
| `testJobSystem.cpp` | Job system |
| `testImgui.cpp` | ImGui integration |
| `testScripting.cpp` | Scripting system |

---

*End of User Manual*
