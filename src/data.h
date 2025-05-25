#include "objects.h"
#include <raylib.h>
enum InteractionMode {
    NORMAL,
    MOVE,
    CREATE_LINE,
    CREATE_POLYLINE,
    SWITCH_TYPE
};

//Custom arrays
typedef struct intarray {
    uint16_t len;
    uint16_t next;
    uint16_t* ptr;
} IntArr;

typedef struct pointarray {
    uint16_t len;
    uint16_t nextAddr;
    PhysPoint* ptr;
} PointArr;

typedef struct linearr {
    uint16_t len;
    uint16_t nextAddr;
    Line* ptr;
} LineArr;

typedef struct circlearray {
    uint16_t len;
    uint16_t nextAddr;
    Circle* ptr;
} CircArr;


//Will be used to store the font internally
const unsigned char fontData[] = {};


struct Time {
        uint64_t prevTime;
        uint64_t currTime;
        double runningFrameTime;
        double timeStep;
        double desiredFrameTime;
        double totalSimTime;
};

struct Cam {
    Camera2D camera;
    float panSpeed;
    float zoomScale;
};

typedef struct simState {
    PointArr points;
    LineArr lines;
    CircArr circles;
    double currTime;
    struct Time time;
    struct Cam cam;
    uint64_t physFrameCounter;
    uint8_t isPaused;
} State;
