#include <math.h>
#include "shapeOps.c"
#include <raylib.h>
#include <stdio.h>
#include <time.h>



/*
double simpson13(double coefficient, double timeStep) {
    return (timeStep / 6.0) * (coefficient + 6 * coefficient +  
}
*/

/*
void simpson13IncrementPos(DoubleVector2 pos, DoubleVector2 vel, DoubleVector2 acc, double timeStep) {
}
*/


void EulerIncrementPosPoint(PhysPoint *point, double timeStep) {

    point->acceleration.x = point->forceAccumulator.x / point->mass;
    point->acceleration.y = point->forceAccumulator.y / point->mass;

    point->position.x += point->velocity.x * timeStep;
    point->velocity.x += point->acceleration.x * timeStep;

    point->position.y += point->velocity.y * timeStep;
    point->velocity.y += point->acceleration.y * timeStep;
}

void EulerIncrementPos(DoubleVector2 pos, DoubleVector2 vel, DoubleVector2 acc, double timeStep) {
    pos.x += vel.x * timeStep;
    vel.x += acc.x * timeStep;

    pos.y += vel.y * timeStep;
    vel.y += acc.y * timeStep;
}

//Calculates the square of the distance between point1 and point2
double measureSquareDist(PhysPoint* point1, PhysPoint* point2) {
    double xDiff = point1->position.x - point2->position.x;
    double yDiff = point1->position.y - point2->position.y;
    return xDiff * xDiff + yDiff * yDiff;
}


//Calculates the distance between point1 and point2
double measureDist(PhysPoint* point1, PhysPoint* point2) {
    return sqrt(measureSquareDist(point1, point2));
}

//Updates the gravitational forces between two objects
void updateGravObjects(PhysPoint* point1, PhysPoint* point2, double timeStep) {
    double force = (GRAV_CONSTANT * point1->mass * point2->mass) / measureSquareDist(point1, point2);
    //printf("f: %.30lf\n", force);
    double dist = measureDist(point1, point2);
    point1->forceAccumulator.x += force * (point2->position.x - point1->position.x)/dist;
    point1->forceAccumulator.y += force * (point2->position.y - point1->position.y)/dist;

    point2->forceAccumulator.x += force * (point1->position.x - point2->position.x)/dist;
    point2->forceAccumulator.y += force * (point1->position.y - point2->position.y)/dist;
}

//Measures the angle formed by point1, mid, and point2
double measureAngle(PhysPoint* point1, PhysPoint* mid, PhysPoint* point2) {
    //Using the law of cosines solved for angle c
    /*
    c^2 = a^2 + b^2 - 2abcos(c)
    2abcos(c) = - c^2 + a^2 + b^2
    c = arccos((-c^2 +a^2 + b^2) / (2 * a * b))
    */
    double aSquare = measureSquareDist(point1, mid);
    double bSquare = measureSquareDist(point2, mid);
    return acos((aSquare + bSquare - measureSquareDist(point1, point2)) / (2 * sqrt(aSquare) * sqrt(bSquare)));
}



//TODO: Save and load from basic JSON or XML
void saveState(char* fileName, void* state, unsigned long size) {
    FILE* saveFile = fopen(fileName, "wb");
    fwrite(state, 1, size, saveFile);
    fclose(saveFile);
}


void* loadState(char* fileName) {
    FILE* saveFile = fopen(fileName, "r");
    int fileSize = fseek(saveFile, SEEK_END, 0);
    void* stateData = malloc(fileSize);
    if (stateData == NULL) {
        return NULL;
    }

    fread(stateData, 1, fileSize, saveFile);
    fclose(saveFile);
    return stateData;
}

State state;

//Updates the simulation time every run
void updateTime() {
    state.time.currTime = clock();
    state.time.timeStep = (double) (state.time.currTime - state.time.prevTime) / CLOCKS_PER_SEC;
    state.time.prevTime = state.time.currTime;
    state.time.runningFrameTime += state.time.timeStep;
}

//Sets up the camera for rendering
Camera2D initializeCamera() {
    Camera2D camera = {0};
    camera.target = (Vector2) {0.0f, 0.0f};
    camera.offset = (Vector2) {GetRenderWidth() / 2.0, GetRenderHeight() / 2.0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    state.cam.zoomScale = 0.1;
    state.cam.panSpeed = 10;
    return camera;
}

//Sets up the simulation, including the camera, window, and state variables
void setup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 1000, "Cooper's Basic Physics Simulator");

    state.cam.camera = initializeCamera();
    state.isPaused = 0;
    //Setting the desired time for each frame in seconds (1 / desired frame rate)
    state.time.desiredFrameTime = 1.0 / 60.0;
    
    //Initializing the State variables
    state.points.len = 20;
    state.points.ptr = malloc(sizeof(PhysPoint) * state.points.len);
    state.points.nextAddr = 0;

    state.circles.len = 20;
    state.circles.ptr = malloc(sizeof(Circle) * state.circles.len);
    state.circles.nextAddr = 0;
}

//Frees memory after execution is completed
void cleanup() {
    free(state.points.ptr);
    free(state.circles.ptr);
}

//Temporary only
int rectX = 0;
int rectY = 0;

void drawUI() {
    const char* framerateText = TextFormat("Render rate: %.0f Hz\nSimulation rate: %.0f kHz", 
            1/(state.time.runningFrameTime), 1.0/(state.time.timeStep * 1000));
    DrawText(framerateText, GetRenderWidth() - MeasureText(framerateText, 20) - 10, 10, 20, WHITE);
}

//Draws each frame
void drawFrame() {
    if (IsWindowResized()) {
        state.cam.camera.offset = (Vector2) {GetRenderWidth() / 2.0, GetRenderHeight() / 2.0};
    }
    ClearBackground((Color) {0, 0, 0, 255});
    BeginDrawing();
    BeginMode2D(state.cam.camera);
    for (int i = 0; i < state.points.nextAddr; i++) {
        DrawCircleLines(state.points.ptr[i].position.x, state.points.ptr[i].position.y, 
                state.points.ptr[i].mass / 10000000, YELLOW);
    }

    DrawRectangle(rectX, rectY, 50, 50, (Color) {120, 200, 50, 255});
    DrawCircle(0, 0, 10.0f, GREEN);
    EndMode2D();
    drawUI();
    EndDrawing();
}

//Updates the physics based on the time step (Calculated and set in the state variable in the UpdateTime function)
void updatePhysics() {
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

    //Incrementing the counter of physics frames passed (for metrics only)
    state.physFrameCounter += 1;
}

//Processes user inputs
void processInputs(void) {
    //Control time pausing
    if (IsKeyPressed(KEY_SPACE)) state.isPaused = !state.isPaused;
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state.cam.camera);

    //Temporary camera position reset
    if (IsKeyDown(KEY_F)) state.cam.camera.target = (Vector2) {rectX, rectY};

    //Camera controls (Pan and zoom)
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        state.cam.camera.zoom += GetMouseWheelMove() * state.cam.zoomScale * state.cam.camera.zoom;
        Vector2 newMouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state.cam.camera);
        //Bias towards mouse
        state.cam.camera.target.x -= newMouseWorldPos.x - mouseWorldPos.x;
        state.cam.camera.target.y -= newMouseWorldPos.y - mouseWorldPos.y;

        if (state.cam.camera.zoom < 0.001) {
            state.cam.camera.zoom = 0.001;
        }
    } else {
        Vector2 mouseDelta = GetMouseWheelMoveV();
        float panScale = state.cam.panSpeed / state.cam.camera.zoom;
        state.cam.camera.target.x -= mouseDelta.x * panScale;
        state.cam.camera.target.y -= mouseDelta.y * panScale;
    }

    if (IsKeyDown(KEY_DOWN)) rectY += 2;
    if (IsKeyDown(KEY_UP)) rectY -= 2;
    if (IsKeyDown(KEY_LEFT)) rectX -= 2;
    if (IsKeyDown(KEY_RIGHT)) rectX += 2;

    //Creating a new physics point at the mouse position. WIP
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        PhysPoint newPoint = {
            .position.x = (double) mouseWorldPos.x,
            .position.y = (double) mouseWorldPos.y,
            .mass = (double) (rand() % 60) * 10000000.0 + 2.0
        };
        int i = PointArrAdd(&state.points, &newPoint);

        Circle newCircle = {
            .centerAddr = i,
            .radius = newPoint.mass / 60000.0
        };
        CircArrAdd(&state.circles, &newCircle);
    }
}
