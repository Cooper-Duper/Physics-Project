#include <math.h>
#include "shapeOps.c"
#include <stdio.h>



//Math utility functions:
//Calculates the square of the distance between point1 and point2
double measureSquareDist(PhysPoint* point1, PhysPoint* point2) {
    double xDiff = point1->position.x - point2->position.x;
    double yDiff = point1->position.y - point2->position.y;
    return xDiff * xDiff + yDiff * yDiff;
}


//Calculates the distance between point1 and point2
double measureDist(PhysPoint* point1, PhysPoint* point2) {
    return sqrt(measureSquareDist(point1, point2));
}

//Updates the gravitational forces between two objects
void updateGravObjects(PhysPoint* point1, PhysPoint* point2) {
    double force = (GRAV_CONSTANT * point1->mass * point2->mass) / measureSquareDist(point1, point2);
    printf("f: %.30lf\n", force);
    double dist = measureDist(point1, point2);
    point1->acceleration.x += (force / point1->mass) * (point2->position.x - point1->position.x)/dist;
    point1->acceleration.y += (force / point1->mass) * (point2->position.y - point1->position.y)/dist;

    point2->acceleration.x += (force / point2->mass) * (point1->position.x - point2->position.x)/dist;
    point2->acceleration.y += (force / point2->mass) * (point1->position.y - point2->position.y)/dist;
}

//Measures the angle formed by point1, mid, and point2
double measureAngle(PhysPoint* point1, PhysPoint* mid, PhysPoint* point2) {
    //Using the law of cosines solved for angle c
    /*
    c^2 = a^2 + b^2 - 2abcos(c)
    2abcos(c) = - c^2 + a^2 + b^2
    c = arccos((-c^2 +a^2 + b^2) / (2 * a * b))
    */
    double aSquare = measureSquareDist(point1, mid);
    double bSquare = measureSquareDist(point2, mid);
    return acos((aSquare + bSquare - measureSquareDist(point1, point2)) / (2 * sqrt(aSquare) * sqrt(bSquare)));
}


//An array of physics points
void* PointArrAdd(PointArr* arr, PhysPoint point) {
    if (arr->nextAddr >= arr->len) {
        arr->len *= 2;
        arr->ptr  = (PhysPoint*) realloc(arr->ptr, sizeof(PhysPoint) * arr->len);
    }
    arr->ptr[arr->nextAddr] = point;
    arr->nextAddr += 1;

    return arr->ptr;
}


//TODO: Save and load from basic JSON or XML
void saveState(char* fileName, void* state, unsigned long size) {
    FILE* saveFile = fopen(fileName, "wb");
    fwrite(state, 1, size, saveFile);
    fclose(saveFile);
}


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
