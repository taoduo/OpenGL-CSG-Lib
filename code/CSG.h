/**
 * Duo Tao
 * CSG.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

#include "CSGError.h"
#include "vector.h"
#include "matrix.h"
#include "CSGNode.h"
#include "primitives.h"

void CSGToMesh(CSGTriangleList *triangles, meshMesh *mesh);

void CSGUnion(meshMesh *meshA, meshMesh *meshB, meshMesh *meshUnion);

void CSGIntersection(meshMesh *meshA, meshMesh *meshB, meshMesh *meshIntersection);

void CSGSubtraction(meshMesh *meshA, meshMesh *meshB, meshMesh *meshSubtraction);