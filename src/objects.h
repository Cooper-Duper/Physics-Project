#define GRAV_CONSTANT 0.000000000066743
#include <stdint.h>
#include "vectors.h"


enum Materials {
    IDEAL_SOLID,
    IDEAL_STRING,
    SPRING
};

enum SpringTypes {
    BI_DIRECTIONAL,
    COMPRESSIVE_ONLY,
    EXPANSIVE_ONLY
};

typedef struct physicsPoint {
    double mass;
    DoubleVector2 position;
    DoubleVector2 velocity;
    DoubleVector2 acceleration;
    DoubleVector2 forceAccumulator;
    enum Materials mat;
    uint8_t isStatic;
} PhysPoint;

typedef struct line {
    PhysPoint* end1;
    PhysPoint* end2;
} Line;


typedef struct spring {
    double constant;
    uint16_t end1Index;
    uint16_t end2Index;
    double InitialLenSquare;
    double InitialLen;
    enum SpringTypes type;
} Spring;


typedef struct circle {
    uint16_t centerAddr;
    double radius;
} Circle;

