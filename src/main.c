#include "utils.c"
#if defined(PLATFORM_WEB)
//Possible emscripten header... Needs to be installed
#endif

int main(int argc, char** argv) {
    setup();
    //Setting up basic scenarios for demos
    //TODO: Move to save files
    if (argc > 1) {
        if (!strcmp(argv[1], "pendulum")) {
            PointArrAdd(&state.points, &(PhysPoint) {
                    .isStatic = 1,
                    });

            PointArrAdd(&state.points, &(PhysPoint) {
                    .position.x = -50,
                    .position.y = 0,
                    .mass = 10,
                    });

            CircArrAdd(&state.circles, &(Circle) {
                    .centerAddr = 1,
                    .radius = 10
                    });

            SpringArrAdd(&state.springs, &(Spring) {
                    .constant = 1000,
                    .end1Index = 0,
                    .end2Index = 1,
                    .InitialLenSquare = 50 * 50,
                    .InitialLen = 50
                    });

            PointArrAdd(&state.points, &(PhysPoint) {
                    .position.x = -100,
                    .position.y = 0,
                    .mass = 10,
                    });
            CircArrAdd(&state.circles, &(Circle) {
                    .centerAddr = 2,
                    .radius = 10
                    });
            SpringArrAdd(&state.springs, &(Spring) {
                    .constant = 1000,
                    .end1Index = 1,
                    .end2Index = 2,
                    .InitialLenSquare = 50 * 50,
                    .InitialLen = 50
                    });
            state.grav.isUniversal = 0;
        }

        if (!strcmp(argv[1], "orbit")) {
            PointArrAdd(&state.points, &(PhysPoint) {
                    .mass = 5972000000000000000000.0,
                    .position.y = 6378137
                    });
            CircArrAdd(&state.circles, &(Circle) {
                    .centerAddr = 0,
                    .radius = 6378137
                    });

            PointArrAdd(&state.points, &(PhysPoint) {
                    .mass = 10,
                    .velocity.x = 7906.05733599
                    });
        }

        if (!strcmp(argv[1], "spring2")) {
            PointArrAdd(&state.points, &(PhysPoint) {
                    .isStatic = 1
                    });
            PointArrAdd(&state.points, &(PhysPoint) {
                    .position.x = 10,
                    .position.y = 3
                    });
            SpringArrAdd(&state.springs, &(Spring) {
                    .end1Index = 0,
                    .end2Index = 1,
                    .InitialLen = measureSpringLen(state.springs.ptr)
                    });
            sscanf(argv[3], "%lf", &state.springs.ptr->constant);
        }
        if (!strcmp(argv[1], "spring1")) {
            PointArrAdd(&state.points, &(PhysPoint) {
                    .isStatic = 1
                    });
            PointArrAdd(&state.points, &(PhysPoint) {
                    .position.y = -3
                    });
            SpringArrAdd(&state.springs, &(Spring) {
                    .end1Index = 0,
                    .end2Index = 1,
                    .InitialLen = measureSpringLen(state.springs.ptr)
                    });
            sscanf(argv[3], "%lf", &state.springs.ptr->constant);
        }
    }
    //Setting ticks per frame
    state.time.desiredTicksPerFrame = 10000;
    if (argc == 3) sscanf(argv[2], "%lu", &state.time.desiredTicksPerFrame);

    //Main loop
    while(!WindowShouldClose()) {
        //Incrementing the simulation time and frame time
        updateTime(state.time.desiredFrameTime / state.time.desiredTicksPerFrame);
        
        //Updating physics when more ticks are needed and the sim is not paused
        if (!state.isPaused && state.time.runningTicks < state.time.desiredTicksPerFrame) updatePhysics();

        //Polling at 60hz to prevent repeat inputs
        //TODO: Higher polling rate
        if (state.time.runningPollingTime >= 1.0/60) {
            processInputs();
            state.time.runningPollingTime = 0;
        }

        //Drawing at specified framerate, or when sim is ready to be displayed
        if (state.time.runningFrameTime >= state.time.desiredFrameTime && (state.time.runningTicks >= state.time.desiredTicksPerFrame || state.isPaused)) {
            drawFrame();
            //Resetting counters
            state.time.runningTicks = 0;
            state.time.runningFrameTime = 0.0;
            state.physFrameCounter = 0;
        }
    }
    //De-allocation and such
    cleanup();
}
