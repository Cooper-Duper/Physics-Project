#include <stdint.h>
#include "vectors.h"

#define GRAV_CONSTANT 0.000000000066743

//The type of material that can be assigned to a line
enum Materials {
    IDEAL_SOLID,
    IDEAL_STRING,
    SPRING
};

//The types of springs (Currently not used)
enum SpringTypes {
    BI_DIRECTIONAL,
    COMPRESSIVE_ONLY,
    EXPANSIVE_ONLY
};

//Data type for storing a point
//TODO: remove and transition to SoA format
typedef struct physicsPoint {
    double mass;
    DoubleVector2 position;
    DoubleVector2 velocity;
    DoubleVector2 acceleration;
    DoubleVector2 forceAccumulator;
    enum Materials mat;
    uint8_t isStatic;
} PhysPoint;

//Line struct
//TODO: remove and transition to SoA format
typedef struct line {
    PhysPoint* end1;
    PhysPoint* end2;
} Line;


//Stores data for a spring
//TODO: remove and transition to SoA format
typedef struct spring {
    double constant;
    uint16_t end1Index;
    uint16_t end2Index;
    double InitialLenSquare;
    double InitialLen;
    double len;
    enum SpringTypes type;
} Spring;


//Data needed to draw a circle
//TODO: remove and transition to SoA format
typedef struct circle {
    uint16_t centerAddr;
    double radius;
} Circle;




typedef enum InteractionMode {
    NORMAL,
    MOVE,
    CREATE_LINE,
    CREATE_POLYLINE,
    SWITCH_TYPE,
    CREATE_CIRCLE,
    CREATE_SPRING
} Mode;

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

typedef struct springarray {
    uint16_t len;
    uint16_t nextAddr;
    Spring* ptr;
} SpringArr;

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
        double timeStep;
        double realElapsedStep;
        double runningPollingTime;
        double runningFrameTime;
        double desiredFrameTime;
        uint64_t desiredTicksPerFrame;
        uint64_t runningTicks;
        double totalSimTime;
};

struct Cam {
    Camera2D camera;
    float panSpeed;
    float zoomScale;
};

struct Energy {
    double springPotential;
    double gravPotential;
    double kinetic;
};

struct GravData {
    uint8_t isUniversal;
    double gravAccel;
    double baseLevel;
};

enum intMethod {
    EULER,
    VERLET
};

typedef struct simState {
    PointArr points;
    LineArr lines;
    SpringArr springs;
    CircArr circles;
    double currTime;
    enum InteractionMode mode;
    enum intMethod intType;
    struct Time time;
    struct Cam cam;
    struct Energy energy;
    struct GravData grav;
    uint64_t physFrameCounter;
    uint8_t isPaused;
} State;
