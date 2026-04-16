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


extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int MAX_ITERATION_COUNT;
extern const float COLOR_CHANGE_COEFFICIENT;
extern const int NO_RETURN_POINT_POW2;


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

#ifdef COLOR_DRAWING

Color MyGetColor(ftype t);
#endif // COLOR_DRAWING



#endif // CALC_H_