#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <raylib.h>
#include <immintrin.h>
#include <assert.h>

#include "fractal.h"


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

const float COLOR_CHANGE_COEFFICIENT = 0.05f;


#if defined(PACKAGE_OPTIMIZATION)

    #include "calc_package.h"
#elif defined(AVX512_OPTIMIZATION)

    #include "calc_avx512.h"
#else

    #include "calc_no_opti.h"
#endif // NO_OPTIMIZATION


static void ComputeFractalData(Image* canvas, CameraContext* ctx);

#ifndef BENCHMARK

static void MyUpdateCamera(CameraContext* cameraCtx);
static void UpdateViewport(CameraContext* cameraCtx);
#endif // BENCHMARK


#ifdef BENCHMARK

unsigned long long RunFractal(unsigned int iterations)
{
    CameraContext cameraCtx = {MIN_X, MAX_X, MIN_Y, MAX_Y, START_X, START_Y, DEFAULT_ZOOM};
    Image canvas = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    unsigned long long start = __rdtsc();
    for(unsigned int i = 0; i < iterations; i++) {
        ComputeFractalData(&canvas, &cameraCtx);
    }
    unsigned long long end = __rdtsc();

    UnloadImage(canvas);
    return end - start;
}
#else

void RunFractal()
{
    CameraContext cameraCtx = {MIN_X, MAX_X, MIN_Y, MAX_Y, START_X, START_Y, DEFAULT_ZOOM};
    Image canvas = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot");
    Texture2D fractalTexture = LoadTextureFromImage(canvas);

    while (!WindowShouldClose()) {
        MyUpdateCamera(&cameraCtx);
        ComputeFractalData(&canvas, &cameraCtx);
        
        UpdateTexture(fractalTexture, canvas.data);
        BeginDrawing();
            DrawTexture(fractalTexture, 0, 0, WHITE);
            DrawFPS(10, 10);
        EndDrawing();
    }
    UnloadTexture(fractalTexture);
    CloseWindow();
    UnloadImage(canvas);
}
#endif // BENCHMARK


static void ComputeFractalData(Image* canvas, CameraContext* cameraCtx)
{
    assert(canvas); assert(canvas->data); assert(cameraCtx);

#if defined(PACKAGE_OPTIMIZATION)

    UpdateImagePackageOptimization(canvas, cameraCtx);
#elif defined(AVX512_OPTIMIZATION)

    UpdateImageAVX512Optimization(canvas, cameraCtx);
#else

    UpdateImageNoOptimization(canvas, cameraCtx);
#endif // AVX512_OPTIMIZATION
}


#ifndef BENCHMARK

static void MyUpdateCamera(CameraContext* cameraCtx)
{
    assert(cameraCtx);

    ftype moveSpeed = MOOV_COEFFICIENT / cameraCtx->zoomLevel;

    if (IsKeyDown(KEY_W)) {
        cameraCtx->centerY -= moveSpeed;
    }
    if (IsKeyDown(KEY_S)) {
        cameraCtx->centerY += moveSpeed;
    }
    if (IsKeyDown(KEY_A)) {
        cameraCtx->centerX -= moveSpeed;
    }
    if (IsKeyDown(KEY_D)) {
        cameraCtx->centerX += moveSpeed;
    }

    if (IsKeyDown(KEY_Q)) {
        cameraCtx->zoomLevel /= 1.1;
    }
    if (IsKeyDown(KEY_E)) {
        cameraCtx->zoomLevel *= 1.1;
    }

    UpdateViewport(cameraCtx);
}


static void UpdateViewport(CameraContext* cameraCtx)
{
    assert(cameraCtx);

    ftype viewportWidth = (MAX_X - MIN_X) / cameraCtx->zoomLevel;
    ftype viewportHeight = viewportWidth * ((ftype)SCREEN_HEIGHT / SCREEN_WIDTH);

    cameraCtx->minX = cameraCtx->centerX - viewportWidth / 2;
    cameraCtx->maxX = cameraCtx->centerX + viewportWidth / 2;
    cameraCtx->minY = cameraCtx->centerY - viewportHeight / 2;
    cameraCtx->maxY = cameraCtx->centerY + viewportHeight / 2;
}


#ifdef COLOR_DRAWING

Color MyGetColor(ftype t) {
    float a[] = {0.5f, 0.5f, 0.5f};
    float b[] = {0.5f, 0.5f, 0.5f};
    float c[] = {1.0f, 1.0f, 1.0f};
    float d[] = AMAZING_COLORS;

    unsigned char r = (unsigned char)(255 * (a[0] + b[0] * cos(2 * PI * (c[0] * t + d[0]))));
    unsigned char g = (unsigned char)(255 * (a[1] + b[1] * cos(2 * PI * (c[1] * t + d[1]))));
    unsigned char b_val = (unsigned char)(255 * (a[2] + b[2] * cos(2 * PI * (c[2] * t + d[2]))));

    return (Color){ r, g, b_val, 255 };
}
#endif // COLOR_DRAWING

#endif // BENCHMARK