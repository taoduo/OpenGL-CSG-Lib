#ifndef CSGERROR_H
#define CSGERROR_H
/**
 * Duo Tao
 * CSGError.h
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

void heapAllocationFailure();

void externalError();

void triangleSplitError();

void debug(char msg[]);

#endif