#include <MotArda/Engine.hpp>
#include <MotArda/Ecs.hpp>
#include <MotArda/Camera.hpp>
#include <MotArda/Systems/RenderSystem.hpp>

#include <memory>

#include <switch.h>
#include <unistd.h>

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
    auto maybeEng = MTRD::MotardaEng::createEngine(1280, 720, "Motarda triangle Switch");

    if (maybeEng.has_value()) {
        auto& eng = maybeEng.value();

        // The main loop will run until the user closes the window
        while (!eng.windowShouldClose()) {
            // Creates a new frame
            eng.windowInitFrame();

            // Ends the frame
            eng.windowEndFrame();
        }

        return 0;
    }

    return 1;
}
