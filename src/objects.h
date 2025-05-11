#define GRAV_CONSTANT 0.000000000066743


typedef struct DoubleVector2 {
    double x;
    double y;
} DoubleVector2;


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
    enum Materials mat;
} PhysPoint;

typedef struct line {
    PhysPoint* end1;
    PhysPoint* end2;
} Line;


typedef struct spring {
    float constant;
    PhysPoint* end1;
    PhysPoint* end2;
    float InitialLenSquare;
    enum SpringTypes type;
} Spring;


typedef struct circle {
    PhysPoint center;
    double radius;
} Circle;

