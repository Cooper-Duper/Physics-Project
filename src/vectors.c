#include "vectors.h"

Vector2 toVec2(DoubleVector2 vec) {
    return (Vector2) {
        .x = (float) vec.x,
        .y = (float) vec.y
    };
}

DoubleVector2 add(DoubleVector2 vec1, DoubleVector2 vec2) {
    return (DoubleVector2) {vec1.x + vec2.x, vec1.y + vec2.y};
}

DoubleVector2 dot(DoubleVector2 vec1, DoubleVector2 vec2) {
    return (DoubleVector2) {vec1.x * vec2.x, vec1.y * vec2.y};
}

DoubleVector2 scale(DoubleVector2 vec, double factor) {
    return (DoubleVector2) {vec.x * factor, vec.y * factor};
}

DoubleVector2 negate(DoubleVector2 vec) {
    return (DoubleVector2) {-vec.x, -vec.y};
}
