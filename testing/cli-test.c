#include <stdio.h>
#include <raylib.h>
#include "../src/utils.c"

int main(int argc, char** argv) {
    double timeStep;
    double desiredTime;
    double logInterval;
    double offset;
    double constant;
    sscanf(argv[1], "%lf", &timeStep);
    sscanf(argv[2], "%lf", &desiredTime);
    sscanf(argv[3], "%lf", &logInterval);
    sscanf(argv[4], "%lf", &offset);
    sscanf(argv[5], "%lf", &constant);

    PhysPoint a = (PhysPoint) {
        .isStatic = true,
    };

    PhysPoint e = (PhysPoint) {
        .position.x = 0,
        .position.y = -10,
        .mass = 10
    };

    Spring s = (Spring) {
        .end1Index = 0,
        .end2Index = 1,
        .constant = constant,
        .InitialLenSquare = measureSquareDist(&a, &e),
        .InitialLen = sqrt(s.InitialLenSquare)
    };

    PhysPoint points[2];
    points[0] = a;
    points[1] = e;
    state.points.ptr = points;

    state.points.ptr[1].position.y -= offset;

    printf("End positions: (%.2lf, %.2lf), (%.2lf, %.2lf) at time t=%.2lf\n", 
            state.points.ptr[0].position.x, state.points.ptr[0].position.y, 
            state.points.ptr[1].position.x, state.points.ptr[1].position.y, 
            state.time.totalSimTime);
    state.time.runningFrameTime = 0;

    while (state.time.totalSimTime <= desiredTime) {
        updateTimeDetached(timeStep);
        ApplySpringForce(&s);

        EulerIncrementPosPoint(state.points.ptr, timeStep);
        EulerIncrementPosPoint(state.points.ptr + 1, timeStep);
        if (state.time.runningFrameTime >= logInterval) {
            printf("Force: %lf\n", s.constant * (s.InitialLen - measureDist(state.points.ptr, state.points.ptr + 1)));
            printf("End positions: (%.2lf, %.2lf), (%.2lf, %.2lf) at time t=%.2lf\n", 
                    state.points.ptr[0].position.x, state.points.ptr[0].position.y, 
                    state.points.ptr[1].position.x, state.points.ptr[1].position.y, 
                    state.time.totalSimTime);
            state.time.runningFrameTime = 0;
            printf("Desired positions: (0, 0), (0, %.2lf)\n", -10 - offset * cos(sqrt(s.constant / e.mass) * state.time.totalSimTime));
        }
    }
}
