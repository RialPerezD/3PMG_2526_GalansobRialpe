# <div align="center">🎮 MotArda Engine <br> Ahora con soporte 2D!!</div>
<div align="center">

![Version](https://img.shields.io/badge/Version-1.0.0-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.0%2B-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-green.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

**Un motor de renderizado 3D, 2D minimalista, potente y flexible construido sobre OpenGL.**
</div>

---

## 📑 Tabla de Contenidos

1. [🚀 Introducción](#-1-introducción)
2. [💻 Requisitos del Sistema](#-2-requisitos-del-sistema)
3. [🔨 Construcción del Proyecto](#-3-construcción-del-proyecto)
4. [📐 Arquitectura del Motor](#-4-arquitectura-del-motor)
5. [🎬 Guía de Inicio](#-5-guía-de-inicio)
6. [📦 Renderizado y Modelos](#-6-renderizado-y-modelos)
7. [🎥 Sistema de Cámara](#-7-sistema-de-cámara)
8. [🧩 Entity Component System (ECS)](#-8-entity-component-system-ecs)
9. [💡 Sistema de Iluminación](#-9-sistema-de-iluminación)
10. [🎨 Modos de Renderizado](#-10-modos-de-renderizado)
11. [🎲 Renderizado 2D y Sprites](#-11-renderizado-2d-y-sprites)
12. [⚡ Sistema de Física (PhysX)](#-12-sistema-de-física-physx)
13. [🐍 Scripting con Lua](#-13-scripting-con-lua)
14. [🎮 Sistema de Entrada](#-14-sistema-de-entrada)
15. [🔍 Referencia API y Troubleshooting](#-15-referencia-api-y-troubleshooting)

---

## 🚀 1. Introducción
MotArda es un motor de renderizado personalizado diseñado para simplificar la creación de aplicaciones 3D y 2D. Ofrece una interfaz intuitiva para la gestión de ventanas, carga de modelos y efectos de iluminación avanzada.

**Características principales:**
* **2D:** Foco en pintado 2D con carga de sprite sheets, transparencias y capas de pintado (como Hollow Knight :D).
* **ECS:** Arquitectura orientada a datos para alto rendimiento.
* **Iluminación:** Soporte para luces *Spot*, *Point* y *Directional*.
* **Sombras:** Mapeo de sombras dinámico integrado.
* **Modelos:** Carga asíncrona de archivos `.obj`.
* **Física:** Integración con PhysX para simulación física.
* **Scripting:** Soporte para Lua para lógica de juego.

---

## 💻 2. Requisitos del Sistema
Para asegurar un rendimiento óptimo, verifica que tu entorno cumpla con lo siguiente:

| Software | Requisito |
| :--- | :--- |
| **OS** | Windows 10/11 |
| **IDE** | Visual Studio 2019 o superior |
| **Herramientas** | Premake & Conan |
| **GPU** | Tarjeta compatible con OpenGL 4.0 |
| **RAM** | Mínimo 4GB |

---

## 🔨 3. Construcción del Proyecto
Sigue estos tres pasos para compilar tu primer proyecto:

1.  **Generar Archivos:** Ejecuta `./tools/genproyect.bat`.
2.  **Abrir Solución:** Abre `./build/MotArda.sln` en Visual Studio.
3.  **Compilar:** Selecciona tu configuración y pulsa **F5**.

---

## 📐 4. Arquitectura del Motor
El corazón de MotArda se divide en componentes modulares:

| Componente | Descripción |
| :--- | :--- |
| `MotardaEng` | Clase principal del motor y gestión de ventana. |
| `ECSManager` | Cerebro del Entity Component System. |
| `Camera` | Gestión de la perspectiva y vista. |
| `PhysxComponent` | Componente para física. |

---

## 🎬 5. Guía de Inicio
### Creación de Ventana Básica
```cpp
#include "MotArda/common/Engine.hpp"

int MTRD::main() {
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "MotArda Window");

    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();

        while (!eng.windowShouldClose()) {
            eng.windowInitFrame();
            // Lógica aquí
            eng.windowEndFrame();
        }
        return 0;
    }
    return 1;
}
```


---

## 📦 6. Renderizado y Modelos

### Carga Asíncrona de Modelos OBJ
El motor permite cargar modelos en segundo plano para evitar bloqueos:
```cpp
std::vector<const char*> objsRoutes = { "model.obj" };
eng.enqueueTask([&]() {
    ObjList = eng.loadObjs(objsRoutes);
    objsLoaded = true;
});
```

### Definición de Materiales
```cpp
Material mat;
mat.diffuse = glm::vec3(1.0f, 0.0f, 0.0f);    // RGB color
mat.specular = glm::vec3(1.0f, 1.0f, 1.0f);   // Specular reflection
mat.shininess = 32.0f;                        // Shininess factor
mat.loadeable = true;
mat.diffuseTexPath = "../assets/textures/blank/blank.jpg";
```


### Geometría Integrada
```cpp
// Generar esfera
ObjItem sphere = eng.generateSphere(radius, widthSegments, heightSegments, texId);

// Generar plano
ObjItem plane = eng.generatePlane(width, height, texId);

// Generar cubo
ObjItem cube = eng.generateCube(size);
```

---

## 🎥 7. Sistema de Cámara
Controla la perspectiva con métodos sencillos:

* **`setPosition(vec3)`**: Ubicación en el mundo.
* **`setTarget(vec3)`**: Punto al que mira la cámara.
* **`rotate(yaw, pitch)`**: Rotación de la vista.
* **`moveForward/Backward(float)`**: Mover adelante/atrás.
* **`moveLeft/Right(float)`**: Mover izquierda/derecha.
* **`moveUp/Down(float)`**: Mover arriba/abajo.

---

## 🧩 8. Entity Component System (ECS)
Gestiona tus objetos mediante componentes especializados:

> 💡 **Tip:** Separa la lógica de movimiento de la representación visual añadiendo distintos componentes a una misma entidad.

| Componente | Función |
| :--- | :--- |
| `TransformComponent` | Define posición, rotación y escala. |
| `RenderComponent` | Contiene referencias a mallas y materiales. |
| `MovementComponent` | Datos de movimiento y animación. |
| `LightComponent` | Convierte la entidad en una fuente de luz. |
| `PhysxComponent` | Comportamiento físico de la entidad. |

### Uso Básico del ECS
```cpp
ECSManager& ecs = eng.getEcs();

// Registrar componentes
ecs.AddComponentType<TransformComponent>();
ecs.AddComponentType<RenderComponent>();
ecs.AddComponentType<MovementComponent>();

// Crear entidad
size_t entity = ecs.AddEntity();

// Añadir componentes
TransformComponent* t = ecs.AddComponent<TransformComponent>(entity);
t->position = glm::vec3(0, 0, 0);
t->scale = glm::vec3(1, 1, 1);
```

---

## 💡 9. Sistema de Iluminación
MotArda soporta iluminación realista mediante tres tipos de luces principales:

* **Puntual (Point):** Emite luz en todas direcciones desde un punto.
* **Focal (Spot):** Luz cónica con decaimiento.
* **Direccional:** Simula la luz solar (rayos paralelos).

### Tipos de Luz
```cpp
// Luz Direccional
lightComp->directionalLights.push_back(
    MTRD::DirectionalLight(direction, color, intensity)
);

// Luz Puntual
lightComp->pointLights.push_back(
    MTRD::PointLight(position, color, intensity, linearAtt, quadraticAtt)
);

// Luz Focal
lightComp->spotLights.push_back(
    MTRD::SpotLight(position, direction, cutoff, outerCutoff, ...)
);
```

---

## 🎨 10. Modos de Renderizado
Cambia el comportamiento visual del motor en tiempo de ejecución:

| Modo | Descripción |
| :--- | :--- |
| `Base` | Renderizado 3D básico sin luces. |
| `LightsWithShadows` | Renderizado completo con iluminación y sombras. |
| `Bidimensional` | Modo ortográfico para sprites 2D. |
| `DeferredWithLights` | Pipeline deferred para múltiples luces. |

```cpp
eng.SetRenderType(MotardaEng::RenderType::DeferredWithLights);
```

---

## 🎲 11. Renderizado 2D y Sprites
MotArda incluye soporte completo para renderizado 2D mediante sprites y sprite sheets.

### Cargar Texturas
```cpp
#include <MotArda/win64/Texture.hpp>

GLuint texture = Texture::LoadTexture("../assets/textures/imagen.png");
```

### Crear Sprites
```cpp
// Sprite estático
Sprite sprite = eng.generateSprite(texture, width, height);

// Sprite sheet animado
Sprite animatedSprite = eng.generateSpriteSheet(
    texture,       // Textura
    frames,        // Total de frames
    frameWidth,    // Ancho de cada frame
    frameHeight,   // Alto de cada frame
    rows,          // Filas en el spritesheet
    cols,          // Columnas en el spritesheet
    fps            // Frames por segundo
);
```

### Animación de Sprites
```cpp
float animTimer = 0.0f;

// En el bucle principal
animTimer += eng.windowGetLastFrameTime();
if (animTimer >= 0.5f) {
    animatedSprite.nextFrame();  // Avanzar al siguiente frame
    animTimer = 0.0f;
}
```

### Mover Sprites
```cpp
TransformComponent* spriteTransform = ecs.GetComponent<TransformComponent>(sprite.getId());

// Controles WASD
if (eng.inputIsKeyPressed(Input::Keyboard::W)) spriteTransform->position.y += speed;
if (eng.inputIsKeyPressed(Input::Keyboard::S)) spriteTransform->position.y -= speed;
if (eng.inputIsKeyPressed(Input::Keyboard::A)) spriteTransform->position.x -= speed;
if (eng.inputIsKeyPressed(Input::Keyboard::D)) spriteTransform->position.x += speed;
```

---

## ⚡ 12. Sistema de Física (PhysX)
MotArda integra el motor de física PhysX para simulación realista.

### Activar Física
```cpp
eng.hasPhysx(true);
```

### Añadir Componente Físico
```cpp
ecs.AddComponentType<PhysxComponent>();
```

### Definir Formas Físicas
```cpp
PhysxComponent* physx = ecs.AddComponent<PhysxComponent>(entity);

// Esfera
physx->shapeType = PhysxShapeType::Sphere;
physx->radius = 0.5f;

// Caja
physx->shapeType = PhysxShapeType::Box;
physx->halfExtents = glm::vec3(1.0f, 0.5f, 1.0f);
```

### Cuerpo Dinámico (se mueve)
```cpp
physx->isDynamic = true;
physx->mass = 1.0f;
```

### Cuerpo Estático (fijo)
```cpp
physx->isDynamic = false;
physx->mass = 0.0f;
```

### Crear Actor Físico
```cpp
TransformComponent* t = ecs.GetComponent<TransformComponent>(entity);
eng.createPhysxActor(*physx, *t);
```

### Ejemplo Completo: Esfera que cae
```cpp
// Esfera dinámica
PhysxComponent* spherePhysx = ecs.AddComponent<PhysxComponent>(sphereEntity);
spherePhysx->shapeType = PhysxShapeType::Sphere;
spherePhysx->radius = 0.5f;
spherePhysx->mass = 1.0f;
spherePhysx->isDynamic = true;
eng.createPhysxActor(*spherePhysx, *sphereTransform);

// Plano estático (suelo)
PhysxComponent* floorPhysx = ecs.AddComponent<PhysxComponent>(floorEntity);
floorPhysx->shapeType = PhysxShapeType::Box;
floorPhysx->halfExtents = glm::vec3(10.0f, 0.05f, 10.0f);
floorPhysx->mass = 0.0f;
floorPhysx->isDynamic = false;
eng.createPhysxActor(*floorPhysx, *floorTransform);
```

---

## 🐍 13. Scripting con Lua
MotArda permite añadir lógica de juego mediante scripts Lua utilizando la librería Sol.

### Incluir Librería
```cpp
#include <sol/sol.hpp>
```

### Inicializar Lua
```cpp
sol::state lua;
lua.open_libraries(sol::lib::base);
```

### Binding de Tipos GLM
```cpp
// Vec2
lua.new_usertype<glm::vec2>("vec2_t", "x", &glm::vec2::x, "y", &glm::vec2::y);

// Vec3
lua.new_usertype<glm::vec3>("vec3_t",
    "x", &glm::vec3::x,
    "y", &glm::vec3::y,
    "z", &glm::vec3::z
);

// Función auxiliar
lua.set_function("vec3", [](float x, float y, float z) {
    return glm::vec3(x, y, z);
});
```

### Binding de Componentes
```cpp
// Transform
lua.new_usertype<TransformComponent>("Transform",
    "position", &TransformComponent::position,
    "rotation", &TransformComponent::rotation,
    "angleRotationRadians", &TransformComponent::angleRotationRadians,
    "scale", &TransformComponent::scale
);

// Movement
lua.new_usertype<MovementComponent>("Movement",
    "position", &MovementComponent::position,
    "rotation", &MovementComponent::rotation,
    "scale", &MovementComponent::scale
);
```

### Binding de Cámara
```cpp
lua.new_usertype<Camera>("Camera",
    "moveForward", &Camera::moveForward,
    "moveBackward", &Camera::moveBackward,
    "moveLeft", &Camera::moveLeft,
    "moveRight", &Camera::moveRight,
    "moveUp", &Camera::moveUp,
    "moveDown", &Camera::moveDown,
    "rotate", &Camera::rotate
);
```

### Sistema de Input en Lua
```cpp
// Crear tabla Input
lua.create_named_table("Input");

// Función para detectar teclas
lua["Input"]["isKeyPressed"] = [](int key) {
    return g_engine->inputIsKeyPressed(static_cast<Input::Keyboard>(key));
};

// Constantes de teclas
lua["KEY_W"] = (int)Input::Keyboard::W;
lua["KEY_S"] = (int)Input::Keyboard::S;
lua["KEY_A"] = (int)Input::Keyboard::A;
lua["KEY_D"] = (int)Input::Keyboard::D;
lua["KEY_Q"] = (int)Input::Keyboard::Q;
lua["KEY_E"] = (int)Input::Keyboard::E;
lua["KEY_I"] = (int)Input::Keyboard::I;
lua["KEY_K"] = (int)Input::Keyboard::K;
lua["KEY_J"] = (int)Input::Keyboard::J;
lua["KEY_L"] = (int)Input::Keyboard::L;
lua["KEY_U"] = (int)Input::Keyboard::U;
lua["KEY_O"] = (int)Input::Keyboard::O;
lua["KEY_N"] = (int)Input::Keyboard::N;
lua["KEY_M"] = (int)Input::Keyboard::M;
```

### Ejemplo de Script Lua
```lua
function update_player(movement)
    if Input.isKeyPressed(KEY_I) then
        movement.position.y = movement.position.y + 0.1
    end
    if Input.isKeyPressed(KEY_K) then
        movement.position.y = movement.position.y - 0.1
    end
    if Input.isKeyPressed(KEY_J) then
        movement.position.x = movement.position.x - 0.1
    end
    if Input.isKeyPressed(KEY_L) then
        movement.position.x = movement.position.x + 0.1
    end
end

function update_camera(camera, speed)
    if Input.isKeyPressed(KEY_W) then
        camera:moveForward(speed)
    end
    if Input.isKeyPressed(KEY_S) then
        camera:moveBackward(speed)
    end
    if Input.isKeyPressed(KEY_A) then
        camera:moveLeft(speed)
    end
    if Input.isKeyPressed(KEY_D) then
        camera:moveRight(speed)
    end
end
```

### Ejecutar Funciones Lua desde C++
```cpp
// Cargar script
sol::load_result chunk = lua.load(lua_program);
chunk();

// Obtener función
sol::protected_function lua_update = lua["update_player"];

// Llamar en el bucle principal
sol::protected_function_result r = lua_update(movementComponent);
if (!r.valid()) {
    sol::error err = r;
    std::cout << "[Lua Error] " << err.what() << std::endl;
}
```

---

## 🎮 14. Sistema de Entrada
Mapeo de teclas recomendado para interacciones utilizado en los ejemplos:

| Tecla | Acción común |
| :--- | :--- |
| **W, A, S, D** | Movimiento lateral y frontal |
| **Q, E** | Movimiento vertical |
| **R, T** | Rotación de cámara |
| **F, G** | Rotación de pitch |
| **I, J, K, L** | Controles alternativos (Lua) |
| **U, O** | Rotación de objeto (Lua) |
| **N, M** | Escalado (Lua) |

---

## 🔍 15. Referencia API y Troubleshooting

### Métodos Útiles de `MotardaEng`
* `windowGetSizeRatio()`: Obtiene el aspect ratio de la ventana.
* `generateSphere/Cube/Plane()`: Crea geometría básica al vuelo.
* `generateSprite/Sheet()`: Crea sprites 2D.
* `SetDebugMode(bool)`: Activa/Desactiva información de depuración.
* `hasPhysx(bool)`: Activa/desactiva el sistema de física.
* `createPhysxActor()`: Crea un actor físico.

### ❓ Solución de Problemas
* **¿Pantalla Negra?** Revisa si la cámara está dentro de un objeto o si olvidaste llamar a `RenderScene()`.
* **¿Fallo de Texturas?** Verifica que las rutas en `Material` sean relativas al ejecutable.
* **¿Sombras no visibles?** Asegúrate de usar el modo `LightsWithShadows`.
* **¿Física no funciona?** Verifica que `eng.hasPhysx(true)` esté activado.
* **¿Lua no responde?** Comprueba que las funciones estén correctamente boundeadas.

---

## 📚 Ejemplos Disponibles

| Archivo | Demuestra |
| :--- | :--- |
| `testWindow.cpp` | Creación básica de ventana. |
| `testTriangle.cpp` | Renderizado de malla personalizada. |
| `testMovTriangle.cpp` | Movimiento de cámara. |
| `testLoadObj.cpp` | Carga de modelos OBJ. |
| `testLights.cpp` | Iluminación y sombras. |
| `testEcs.cpp` | Sistema ECS. |
| `test2D.cpp` | Renderizado 2D y sprites. |
| `testPhysx.cpp` | Física con PhysX. |
| `testScripting.cpp` | Scripting con Lua. |
| `testDefferred.cpp` | Renderizado deferred. |

---
<div align="center">
  <sub>MotArda Engine Manual - 2026. Wigili wigili, your game is now my property.</sub>
</div>