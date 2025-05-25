#include "vectors.h"

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
