#include <raylib.h>
#include <stdio.h>
#include <limits.h>
#include "utils.c"
#include <time.h>
#if defined(PLATFORM_WEB)
//Possible emscripten header... Needs to be installed
#endif


//Global variables (State stays, rectX and Y are temporary
State state;
int rectX = 0;
int rectY = 0;

long physFrameCounter = 0;

void updateTime() {
        state.time.currTime = clock();
        state.time.timeStep = (double) (state.time.currTime - state.time.prevTime) / CLOCKS_PER_SEC;
        state.time.prevTime = state.time.currTime;
        state.time.runningFrameTime += state.time.timeStep;
}


void initializeState() {
    //Setting the desired time for each frame in seconds (1 / desired frame rate)
    state.time.desiredFrameTime = 1.0 / 60.0;
    //Initializing the State variables
    PointArr points = (PointArr) {
        .len = 10,
            .ptr = malloc(points.len * sizeof(PhysPoint)),
            .nextAddr = 0
    };

    state.points.len = 20;
    state.points.ptr = malloc(sizeof(PhysPoint) * state.points.len);
    state.points.nextAddr = 0;

    state.circles.len = 20;
    state.circles.ptr = malloc(sizeof(Circle) * state.circles.len);
    state.circles.nextAddr = 0;
}


void drawFrame(Camera2D camera) {
        ClearBackground((Color) {0, 0, 0, 255});
        BeginDrawing();
        DrawLine(0, 0, GetMouseX(), GetMouseY(), (Color) {255, 255, 255, 255});
        BeginMode2D(camera);
        for (int i = 0; i < state.points.nextAddr; i++) {
            DrawCircleLines(state.points.ptr[i].position.x, state.points.ptr[i].position.y, 
                    state.points.ptr[i].mass/10000000, YELLOW);
        }

        DrawRectangle(rectX, rectY, 50, 50, (Color) {120, 200, 50, 255});
        DrawCircle(0, 0, 10.0f, GREEN);
        EndMode2D();
        EndDrawing();
}

Camera2D initializeCamera() {
    Camera2D camera = {0};
    camera.target = (Vector2) {0.0f, 0.0f};
    camera.offset = (Vector2) {200.0f, 200.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    return camera;
}


int main(int argc, char** argv) {
    //Window setup
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(500, 500, "Cooper's Basic Physics Simulator");
    
    //Camera setup
    Camera2D camera = initializeCamera();
    //Camera controls setup. Should be organized better
    const float panSpeed = 6.0f;
    float panScale;
    const float zoomScale = 0.2f;

    //Variable for mouse
    Vector2 mouseDelta;

    initializeState();

    //Main loop
    while(!WindowShouldClose()) {
        //Incrementing the simulation time
        updateTime();



        //Updating accelerations based on gravity
        for (int i = 0; i < state.circles.nextAddr; i++) {
            for (int j = i + 1; j < state.circles.nextAddr; j++) {
                updateGravObjects(state.points.ptr + state.circles.ptr[i].centerAddr, state.points.ptr + state.circles.ptr[j].centerAddr, state.time.timeStep);
            }
        }

        //Updating positions based on acceleration and velocity
        for (int i = 0; i < state.points.nextAddr; i++) {
            EulerIncrementPosPoint(&state.points.ptr[i], state.time.timeStep);
        }

        physFrameCounter += 1;
        //Drawing the frame if sufficient time has passed
        if (state.time.runningFrameTime >= state.time.desiredFrameTime) {
            drawFrame(camera);
            //printf("Physics frames per second: %f\nFrames per second: %f\n", (float) physFrameCounter / state.time.runningFrameTime, 1.0 / state.time.runningFrameTime);
            state.time.runningFrameTime = 0.0;
            physFrameCounter = 0;

            if (IsKeyDown(KEY_F)) camera.target = (Vector2) {rectX, rectY};
            if (IsKeyDown(KEY_LEFT_CONTROL)) {
                camera.zoom += GetMouseWheelMove() * zoomScale * camera.zoom;
                if (camera.zoom < 0.001) {
                    camera.zoom = 0.001;
                }
            } else {
                mouseDelta = GetMouseWheelMoveV();
                panScale = panSpeed / camera.zoom;
                camera.target.x -= mouseDelta.x * panScale;
                camera.target.y -= mouseDelta.y * panScale;
            }
            if (IsKeyDown(KEY_DOWN)) rectY += 2;
            if (IsKeyDown(KEY_UP)) rectY -= 2;
            if (IsKeyDown(KEY_LEFT)) rectX -= 2;
            if (IsKeyDown(KEY_RIGHT)) rectX += 2;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), camera);
                PhysPoint newPoint = {
                    .position.x = (double) worldPos.x,
                    .position.y = (double) worldPos.y,
                    .mass = (double) (random() % 60) * 10000000.0 + 2.0
                };
                int i = PointArrAdd(&state.points, &newPoint);

                Circle newCircle = {
                    .centerAddr = i,
                    .radius = newPoint.mass / 60000.0
                };
                CircArrAdd(&state.circles, &newCircle);
            }
        }
    }
}
