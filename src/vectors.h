typedef struct DoubleVector2 {
    double x;
    double y;
} DoubleVector2;

DoubleVector2 add(DoubleVector2 vec1, DoubleVector2 vec2);
DoubleVector2 dot(DoubleVector2 vec1, DoubleVector2 vec2);
DoubleVector2 scale(DoubleVector2 vec, double factor);
DoubleVector2 negate(DoubleVector2 vec);
