#include "objects.h"
enum InteractionMode {
    NORMAL,
    MOVE,
    CREATE_LINE,
    CREATE_POLYLINE,
    SWITCH_TYPE
};

//Custom arrays
typedef struct intarray {
    int len;
    int next;
    int* ptr;
} IntArr;

typedef struct pointarray {
    int len;
    int nextAddr;
    PhysPoint* ptr;
} PointArr;

typedef struct linearr {
    int len;
    int nextAddr;
    Line* ptr;
} LineArr;

typedef struct circlearray {
    int len;
    int nextAddr;
    Circle* ptr;
} CircArr;


//Will be used to store the font internally
const unsigned char fontData[] = {};

typedef struct simState {
    PointArr points;
    LineArr lines;
    CircArr circles;
    double currTime;
} State;
