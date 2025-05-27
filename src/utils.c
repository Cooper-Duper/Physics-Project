#include <math.h>
#include "shapeOps.c"
#include <raylib.h>
#include <time.h>
#include <stdio.h>
State state;

void RK4IncrementPosPoint(PhysPoint *point, double timeStep) {
    //Do nothing if it is a static point
    if (point->isStatic) return;
}

//Using Verlet integration
//This uses the "velocity" field to instead store the previous postion
//BE WARNED
void VerletIncrementPosPoint(PhysPoint* point, double timeStep, double dampening) {
    if (point->isStatic) return;

    point->acceleration.x = point->forceAccumulator.x / point->mass;
    point->acceleration.y = point->forceAccumulator.y / point->mass;
    point->forceAccumulator.x = 0;
    point->forceAccumulator.y = 0;

    DoubleVector2 temp = point->position; 
    point-> position.x = 2 * point->position.x - dampening * point->velocity.x + point->acceleration.x * timeStep * timeStep;
    point-> position.y = 2 * point->position.y - dampening * point->velocity.y + point->acceleration.y * timeStep * timeStep;
    point->velocity = temp;
}

void EulerIncrementPosPoint(PhysPoint *point, double timeStep) {
    //Do nothing if it is a static point
    if (point->isStatic) return;

    //Kinetic energy (from 1/2mv^2, where v.x^2 * v.y^2 = v^2 from the Pythagorean Theorem)
    state.energy.kinetic += 0.5 * point->mass * (point->velocity.x * point->velocity.x + point->velocity.y * point->velocity.y);

    point->acceleration.x = point->forceAccumulator.x / point->mass;
    point->acceleration.y = point->forceAccumulator.y / point->mass;
    point->forceAccumulator.x = 0;
    point->forceAccumulator.y = 0;

    point->velocity.x += point->acceleration.x * timeStep;
    point->position.x += point->velocity.x * timeStep;

    point->velocity.y += point->acceleration.y * timeStep;
    point->position.y += point->velocity.y * timeStep;
}


//Calculates the square of the distance between point1 and point2
//Useful to avoid the sqrt() if it is not needed
double measureSquareDist(PhysPoint* point1, PhysPoint* point2) {
    double xDiff = point1->position.x - point2->position.x;
    double yDiff = point1->position.y - point2->position.y;
    return xDiff * xDiff + yDiff * yDiff;
}


//Calculates the distance between point1 and point2
double measureDist(PhysPoint* point1, PhysPoint* point2) {
    return sqrt(measureSquareDist(point1, point2));
}

//Measures the angle formed by point1, mid, and point2
double MeasureAngle(PhysPoint* point1, PhysPoint* mid, PhysPoint* point2) {
    //Using the law of cosines solved for angle c
    double aSquare = measureSquareDist(point1, mid);
    double bSquare = measureSquareDist(point2, mid);
    return acos((aSquare + bSquare - measureSquareDist(point1, point2)) / (2 * sqrt(aSquare) * sqrt(bSquare)));
}

//Resolving collision between two circles
void CheckResolveCircleCircleCollision(Circle* circ1, Circle*circ2) {
    //Detecting collision
    if (measureSquareDist(state.points.ptr + circ1->centerAddr, state.points.ptr + circ2->centerAddr) < (circ1->radius + circ2->radius) * (circ1->radius * circ2->radius)) {

    }
}

//Resolving collision between a circle and the floor
void CheckResolveCircleFloorCollision(Circle* circ) {
    //Detecting collision
    if (state.points.ptr[circ->centerAddr].position.y - circ->radius < state.grav.baseLevel) {
    }
}


double measureSpringLen(Spring* spring) {
    return measureDist(state.points.ptr + spring->end1Index, state.points.ptr + spring->end2Index);
}

//Spring forces between two physics points
void ApplySpringForce(Spring* spring) {
    spring->len = measureDist(state.points.ptr + spring->end1Index, state.points.ptr + spring->end2Index);

    //Energy (from 1/2kx^2)
    state.energy.springPotential += 0.5 * spring->constant * (-spring->InitialLen + spring->len) * (-spring->InitialLen + spring->len);

    //Hooke's Law
    double forceMagnitude = -spring->constant * (-spring->InitialLen + spring->len);

    //Splitting into components using sin and cos ratios
    DoubleVector2 point2Force = (DoubleVector2) {
        .x = forceMagnitude * ((state.points.ptr + spring->end2Index)->position.x - (state.points.ptr + spring->end1Index)->position.x) / spring->len,
            .y = forceMagnitude * ((state.points.ptr + spring->end2Index)->position.y - (state.points.ptr + spring->end1Index)->position.y) / spring->len,
    };

    //Adding forces to the accumulators
    (state.points.ptr + spring->end1Index)->forceAccumulator = add((state.points.ptr + spring->end1Index)->forceAccumulator, negate(point2Force));
    (state.points.ptr + spring->end2Index)->forceAccumulator = add((state.points.ptr + spring->end2Index)->forceAccumulator, point2Force);
}

//Updates the gravitational forces between two objects
void ApplyGravForceObjectsUniversal(PhysPoint* point1, PhysPoint* point2) {
    //Defining/calculating needed values
    double sqDist = measureSquareDist(point1, point2);
    double constMass = GRAV_CONSTANT * point1->mass * point2->mass;
    double force = (constMass) / sqDist;
    double dist = sqrt(sqDist);

    //Adding energy (from U_g equation)
    state.energy.gravPotential += constMass / dist;

    //Updating force on each point
    double xForce = force * (point2->position.x - point1->position.x)/dist;
    double yForce = force * (point2->position.y - point1->position.y)/dist;
    /*
       printf("%lf, (%lf, %lf)\n", force, xForce, yForce);
       printf("%lf, %lf\n", point1->mass, point2->mass);
       printf("(%lf, %lf), (%lf, %lf)\n\n", point1->position.x, point1->position.y, point2->position.x, point2->position.y);
       */
    point1->forceAccumulator.x += xForce;
    point1->forceAccumulator.y += yForce;

    point2->forceAccumulator.x += -xForce;
    point2->forceAccumulator.y += -yForce;
}

void ApplyGravForceObjectConstant(PhysPoint* point) {
    if (point->isStatic) return;

    state.energy.gravPotential += point->mass * state.grav.gravAccel * (state.grav.baseLevel - point->position.y);
    point->forceAccumulator.y += state.grav.gravAccel * point->mass;
}



//Updates the simulation time every run
void updateTime() {
    state.time.currTime = clock();
    state.time.timeStep = (double) (state.time.currTime - state.time.prevTime) / CLOCKS_PER_SEC;
    state.time.prevTime = state.time.currTime;
    state.time.runningFrameTime += state.time.timeStep;
    state.time.totalSimTime += state.time.timeStep;
    state.time.runningPollingTime += state.time.timeStep;
}

//Updates the time using a set frame time
void updateTimeDetached(double timeStep) {
    state.time.currTime = clock();
    state.time.realElapsedStep = (double) (state.time.currTime - state.time.prevTime) / CLOCKS_PER_SEC;
    state.time.prevTime = state.time.currTime;
    state.time.runningFrameTime += state.time.realElapsedStep;
    state.time.runningPollingTime += state.time.realElapsedStep;
    state.time.timeStep = timeStep;
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
    state.isPaused = 1;
    //Setting the desired time for each frame in seconds (1 / desired frame rate)
    state.time.desiredFrameTime = 1.0 / 60.0;

    state.grav.isUniversal = 1;
    state.grav.baseLevel = 500;
    state.grav.gravAccel = 9.8;
    //Initializing the State variables
    state.points.len = 20;
    state.points.ptr = malloc(sizeof(PhysPoint) * state.points.len);
    state.points.nextAddr = 0;

    state.circles.len = 20;
    state.circles.ptr = malloc(sizeof(Circle) * state.circles.len);
    state.circles.nextAddr = 0;

    state.springs.len = 20;
    state.springs.ptr = malloc(sizeof(Spring) * state.springs.len);
    state.springs.nextAddr = 0;
}

//Frees memory after execution is completed
void cleanup() {
    free(state.points.ptr);
    free(state.circles.ptr);
    free(state.springs.ptr);
}

void drawUI() {
    uint8_t activeMode;

    switch (state.mode) {
        case NORMAL:
            activeMode = 0;
            break;
        case MOVE:
            activeMode = 1;
            break;
        case CREATE_CIRCLE:
            activeMode = 2;
            break;
        case CREATE_SPRING:
            activeMode = 3;
            break;

            //Should never get here
        default:
            exit(1);
            break;
    };

    const char* modes[] = {"Normal  ", "Move  ", "Create Circle  ", "Create Spring"};
    int positions[] = {0, positions[0] + MeasureText(modes[0], 20), positions[1] + MeasureText(modes[1], 20), positions[2] + MeasureText(modes[2], 20)};
    for (uint8_t i = 0; i < 4; i += 1) {
        Color color;
        if (activeMode == i) color = WHITE; else color = GRAY;
        DrawText(modes[i], 10 + positions[i], 5, 20, color);
    }

    int margin = 10;
    int rightUIWidth = 300;
    Vector2 rightUIPosition = (Vector2) {margin, 15 + margin};

    const char* framerateText = TextFormat("Render rate: %.0f Hz",  
            1/(state.time.runningFrameTime));
    DrawText(framerateText, margin + rightUIPosition.x, rightUIPosition.y, 20, WHITE);

    const char* simText;
    if (state.isPaused) simText = "Simulation is paused";
    else simText = TextFormat("Simulation rate: %.0f kHz", 
            1.0/(state.time.timeStep * 1000));
    DrawText(simText, margin + rightUIPosition.x, 20 + rightUIPosition.y, 20, WHITE);

    const char* energyText = TextFormat("Total energy: %.4lf J\n - Kinetic: %.4lf J\n - Spring: %.4lf J\n - Grav: %.4lf J",
            state.energy.gravPotential + state.energy.kinetic + state.energy.springPotential,
            state.energy.kinetic, state.energy.springPotential, state.energy.gravPotential);
    DrawText(energyText, margin + rightUIPosition.x, 50 + rightUIPosition.y, 20, WHITE);

    const char* timeText = TextFormat("Elapsed simulation time: %lf", state.time.totalSimTime);
    DrawText(timeText, margin + rightUIPosition.x, 170, 20, LIGHTGRAY);

    const char* gravStates[] = {"Directional Gravity", "Universal Gravity"};
    DrawText(gravStates[state.grav.isUniversal], 300, rightUIPosition.y, 20, WHITE);

    DrawText("Integration method: Euler", 300, rightUIPosition.y + 20, 20, WHITE);
}

void drawSpring(Spring* spring) {
        Vector2 end1Pos = toVec2(state.points.ptr[spring->end1Index].position);
        Vector2 end2Pos = toVec2(state.points.ptr[spring->end2Index].position);
        DrawLineEx(end1Pos, end2Pos, 3, ORANGE);
}


void drawAxisGrid() {
    Vector2 leftEdge = GetScreenToWorld2D((Vector2) {0, 0}, state.cam.camera);
    Vector2 rightEdge = GetScreenToWorld2D((Vector2) {GetRenderWidth(), GetRenderHeight()}, state.cam.camera);//(Vector2) {GetRenderWidth(), GetRenderHeight()}, state.cam.camera);
    for (int x = (int) leftEdge.x / 10; x <= rightEdge.x; x += 10) {
        Color color;
        color = LIGHTGRAY;
        int thickness = 1;
        if (x % 100 == 0) thickness = 3;
        if (x == 0) {
            color = (Color) {200, 200, 200, 255};
            thickness = 5;
        }
        DrawLineEx(GetWorldToScreen2D((Vector2) {x, leftEdge.y}, state.cam.camera), GetWorldToScreen2D((Vector2) {x, rightEdge.y}, state.cam.camera), thickness, color);
    }
    /*
    for (int y = (int) leftEdge.y % 10; y <= (int) rightEdge.y % 10 + 10; y += 10) {
        Color color;
        color = LIGHTGRAY;
        int thickness = 1;
        if (y % 100 == 0) thickness = 3;
        if (y == 0) {
            color = (Color) {200, 200, 200, 255};
            thickness = 5;
        }
        DrawLineEx((Vector2) {leftEdge.x, y}, (Vector2) {rightEdge.x, y}, thickness, color);
    }
    */
}

//Draws each frame
void drawFrame() {
    if (IsWindowResized()) {
        state.cam.camera.offset = (Vector2) {GetRenderWidth() / 2.0, GetRenderHeight() / 2.0};
    }
    ClearBackground((Color) {0, 0, 0, 255});
    BeginDrawing();
    //drawAxisGrid();
    BeginMode2D(state.cam.camera);

    if (state.grav.isUniversal == 0) {
        Vector2 leftEdge = GetScreenToWorld2D((Vector2) {0, 0}, state.cam.camera);
        Vector2 rightEdge = GetScreenToWorld2D((Vector2) {GetRenderWidth(), GetRenderHeight()}, state.cam.camera);//(Vector2) {GetRenderWidth(), GetRenderHeight()}, state.cam.camera);
        DrawRectangle(leftEdge.x, state.grav.baseLevel, rightEdge.x - leftEdge.x, rightEdge.y, BROWN);
    }


    //Drawing circles
    for (int i = 0; i < state.circles.nextAddr; i++) {
        DrawCircle(state.points.ptr[state.circles.ptr[i].centerAddr].position.x, 
                state.points.ptr[state.circles.ptr[i].centerAddr].position.y, 
                state.circles.ptr[i].radius, YELLOW);
    }


    //Drawing springs
    for (int i = 0; i < state.springs.nextAddr; i++)  {
        drawSpring(state.springs.ptr + i);
    }
    
    //Drawing point markers
    for(int i = 0; i < state.points.nextAddr; i+=1) {
        Color color;
        if (state.points.ptr[i].isStatic) color = GREEN;
        else color = RED;
        DrawCircle(state.points.ptr[i].position.x, state.points.ptr[i].position.y, 7 / state.cam.camera.zoom, color);
    }
    EndMode2D();
    drawUI();
    EndDrawing();
    
}

//Updates the physics based on the time step (Calculated and set in the state variable in the UpdateTime function)
void updatePhysics() {
    state.time.runningTicks += 1;
    state.time.totalSimTime += state.time.timeStep;
    //Resetting energy counters
    state.energy.gravPotential = 0;
    state.energy.springPotential = 0;
    state.energy.kinetic = 0;

    //Updating accelerations based on gravity
    if (state.grav.isUniversal) {
        for (int i = 0; i < state.points.nextAddr; i++) {
            for (int j = i + 1; j < state.points.nextAddr; j++) {
                ApplyGravForceObjectsUniversal(state.points.ptr + i, state.points.ptr + j);
            }
        }
    } else {
        for (int i = 0; i < state.points.nextAddr; i++) {
            ApplyGravForceObjectConstant(state.points.ptr + i);
        }
    }

    for (int i = 0; i < state.springs.nextAddr; i += 1) {
        ApplySpringForce(state.springs.ptr + i);
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
    int input;
    int lmb = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    int rmb = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    Vector2 mouseWheelDelta = GetMouseWheelMoveV();
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state.cam.camera);

    do {

        //Control time pausing
        if (input == KEY_SPACE) state.isPaused = !state.isPaused;

        if (state.mode != NORMAL) state.isPaused = 1;
        if (state.mode != NORMAL && (input == KEY_ESCAPE || input == KEY_CAPS_LOCK)) state.mode = NORMAL;

        switch (state.mode) {
            case NORMAL:

                if (input == KEY_G) state.grav.isUniversal = !state.grav.isUniversal;
                //Switching integration methods
                /* Work in progress
                if (input == KEY_I) {
                    if (state.intType == EULER) {
                        
                    }
                }
                */

                //Camera controls (Pan and zoom)
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    state.cam.camera.zoom += (int) mouseWheelDelta.y * state.cam.zoomScale * state.cam.camera.zoom;
                    Vector2 newMouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state.cam.camera);
                    //Bias towards mouse
                    state.cam.camera.target.x -= newMouseWorldPos.x - mouseWorldPos.x;
                    state.cam.camera.target.y -= newMouseWorldPos.y - mouseWorldPos.y;

                    if (state.cam.camera.zoom < 0.000000001) {
                        state.cam.camera.zoom = 0.000000001;
                    }
                } else {
                    float panScale = state.cam.panSpeed / state.cam.camera.zoom;
                    state.cam.camera.target.x -= mouseWheelDelta.x * panScale;
                    state.cam.camera.target.y -= mouseWheelDelta.y * panScale;
                }

                //Camera reset
                if (input == KEY_R) {
                    state.cam.camera.target = (Vector2) {0, 0};
                    state.cam.camera.zoom = 1.0;
                }

                if (input == KEY_M) state.mode = MOVE;
                if (input == KEY_C) state.mode = CREATE_CIRCLE;

                break;
            case CREATE_CIRCLE:
                if (lmb) {
                    PhysPoint newPoint = {
                        .position.x = (double) mouseWorldPos.x,
                        .position.y = (double) mouseWorldPos.y,
                        .mass = (double) (rand() % 60) * 10000000.0 + 2.0
                    };
                    int i = PointArrAdd(&state.points, &newPoint);

                    Circle newCircle = {
                        .centerAddr = i,
                        .radius = newPoint.mass / 60000000.0
                    };
                    CircArrAdd(&state.circles, &newCircle);
                }
                break;

            default:
                break;
        }
    } while ((input = GetKeyPressed()) != 0);
}

//TODO
void saveState(char* fileName, void* state, unsigned long size) {
    FILE* saveFile = fopen(fileName, "wb");
    fwrite(state, 1, size, saveFile);
    fclose(saveFile);
}


//TODO
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
