#ifndef CALC_H_
#define CALC_H_


#include <raylib.h>


typedef double ftype;

typedef struct {
    ftype minX;
    ftype maxX;
    ftype minY;
    ftype maxY;
    ftype centerX;
    ftype centerY;
    ftype zoomLevel;
} CameraContext;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const ftype DEFAULT_ZOOM = 1.0;
const ftype MOOV_COEFFICIENT = 0.05;

const ftype MIN_X = -2.0;
const ftype MAX_X = 1.0;
const ftype MIN_Y = -1.0;
const ftype MAX_Y = 1.0;

const ftype START_X = -0.5;
const ftype START_Y = 0;

const int MAX_ITERATION_COUNT = 256;
const int NO_RETURN_POINT_POW2 = 4.0;

const int DOUBLE_PACK_SIZE = 8;

// Если < 0.05, то цвета будут меняться плавно, если больше - резко
const float COLOR_CHANGE_COEFFICIENT = 0.05f;


// Цветовые схемы для множества Мандельброта
#define AMAZING_COLORS {0.00f, 0.10f, 0.20f}
#define FIRE_COLORS {0.00f, 0.07f, 0.12f}
#define RAINBOW_COLORS {0.00f, 0.33f, 0.67f}
#define ICE_COLORS {0.30f, 0.20f, 0.20f}


#ifdef BENCHMARK

unsigned long long RunFractal(unsigned int iterations);
#else

void RunFractal();
#endif // BENCHMARK


#endif // CALC_H_