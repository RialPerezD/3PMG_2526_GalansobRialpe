#include "MotArda/common/Engine.hpp"
#include <MotArda/common/Systems/OnlineSystem.hpp>

int MTRD::main() {
    auto maybeEng = MTRD::MotardaEng::createEngine(800, 600, "Motarda Online");
    if (!maybeEng.has_value()) return 1;
    auto& eng = maybeEng.value();

    eng.SetDebugMode(true);
    OnlineSystem* onlineSystem = eng.ActivateOnlineMode();

    if (!onlineSystem) return 1;

    while (!eng.windowShouldClose()) {
        eng.windowInitFrame();

        eng.windowEndFrame();
    }

    onlineSystem->Shutdown();
    return 0;
}
