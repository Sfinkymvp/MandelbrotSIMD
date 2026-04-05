#ifndef CALC_H_
#define CALC_H_


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


void DrawFractal();


#endif // CALC_H_