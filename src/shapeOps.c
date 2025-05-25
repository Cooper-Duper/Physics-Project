#include "data.h"
#include <stdlib.h>
#include <string.h>


int PointArrAdd(PointArr *arr, PhysPoint* toAdd) {
    if (arr->nextAddr >= arr->len - 1) {
        arr->len *= 2;
        void* e = realloc((void*) arr->ptr, arr->len * sizeof(PhysPoint));
        if (e == NULL) return -1;
        arr->ptr = (PhysPoint*) e;
    }
    arr->ptr[arr->nextAddr] = *toAdd;
    arr->nextAddr += 1;
    return arr->nextAddr - 1;
}

int CircArrAdd(CircArr *arr, Circle* toAdd) {
    if (arr->nextAddr >= arr->len - 1) {
        arr->len *= 2;
        void* e = realloc((void*) arr->ptr, arr->len * sizeof(Circle));
        if (e == NULL) return -1;
        arr->ptr = (Circle*) e;
    }
    arr->ptr[arr->nextAddr] = *toAdd;
    arr->nextAddr += 1;
    return arr->nextAddr - 1;
}

//Creates a line between two PhysPoints
Line* createLine(PhysPoint* point1, PhysPoint* point2) {
    Line* line = (Line*) malloc(sizeof(Line));
    line -> end1 = point1;
    line -> end2 = point2;
    return line;
}

//Appends a line to the end of a polyline
void polylineAppend(LineArr current, Line* new) {
    //Doubles array size if it is too small
    if (current.nextAddr >= current.len) {
        void* success = realloc((void*) current.ptr, current.len * 2);
        if (success == NULL) {
            exit(1);
        }
    }

    current.ptr[current.nextAddr] = *new;
    current.nextAddr += 1;
}

//Creates a polyline from an array of line pointers
LineArr polylineCreate(Line** lines, int num) {
    Line** array = malloc(sizeof(Line*) * num);
    memcpy(array, lines, num * sizeof(Line*));
    LineArr result = {.len = num, .nextAddr = num, .ptr = *array};
    return result;
}

//Determines if a polyline is closed (If the last pointer in the array is the same as the first)
int polylineIsClosed(LineArr testLine) {
    return &testLine.ptr[testLine.nextAddr - 1] == &testLine.ptr[0];
}
