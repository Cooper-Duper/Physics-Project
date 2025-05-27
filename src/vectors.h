#include <raylib.h>
typedef struct doubVec2 {
    double x;
    double y;
} DoubleVector2;

Vector2 toVec2(DoubleVector2 vec);
DoubleVector2 add(DoubleVector2 vec1, DoubleVector2 vec2);
DoubleVector2 dot(DoubleVector2 vec1, DoubleVector2 vec2);
DoubleVector2 scale(DoubleVector2 vec, double factor);
DoubleVector2 negate(DoubleVector2 vec);
