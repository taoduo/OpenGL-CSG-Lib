#ifndef CSGPLANE_H
#define CSGPLANE_H
/**
 * Duo Tao
 * CSGPlane.h
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>
#include "CSGError.h"

#define EPSILON 1E-5

struct CSGTriangleList;
typedef struct CSGTriangleList CSGTriangleList;

typedef struct CSGPlane {
	GLdouble normal[3];
	GLdouble w; // distance from the origin. Positive on the side of normal.
} CSGPlane;

#include "CSGTriangle.h"
#include "CSGTriangleList.h"
#include "CSGVertex.h"
#include "CSGVertexList.h"

CSGPlane *CSGPlaneInitialize(GLdouble normal[3], GLdouble w);

CSGPlane *CSGPlaneClone(CSGPlane *plane);

CSGPlane *CSGPlaneFromPoints(CSGVertex *A, CSGVertex *B, CSGVertex *C);

void CSGPlaneDestroy(CSGPlane *plane);

void CSGPlaneFlip(CSGPlane *plane);

void CSGPlaneSplitTriangle(CSGPlane *plane, CSGTriangle *triangle,
	CSGTriangleList *coplanarFront, CSGTriangleList *coplanarBack, CSGTriangleList *front, CSGTriangleList *back);

void CSGPlaneDump(CSGPlane *plane);
#endif