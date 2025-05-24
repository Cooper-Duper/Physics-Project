#include <raylib.h>
#include <limits.h>
#include "utils.c"
#if defined(PLATFORM_WEB)
//Possible emscripten header... Needs to be installed
#endif

int main(int argc, char** argv) {
    setup();
    //Main loop
    while(!WindowShouldClose()) {
        //Incrementing the simulation time and frame time
        updateTime();
        if (!state.isPaused) updatePhysics();
        
        //Drawing and polling inputs at 60hz
        if (state.time.runningFrameTime >= state.time.desiredFrameTime) {
            processInputs();
            drawFrame();
            //Resetting counters
            state.time.runningFrameTime = 0.0;
            state.physFrameCounter = 0;
        }
    }
    cleanup();
}
