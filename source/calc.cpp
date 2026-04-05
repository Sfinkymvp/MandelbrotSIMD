#include <stdio.h>
#include <raylib.h>
#include <assert.h>

#include "calc.h"


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


static void UpdateImage(Image* canvas, const CameraContext* cameraCtx);
static void UpdateCamera(CameraContext* cameraCtx);
static void UpdateViewport(CameraContext* cameraCtx);


void DrawFractal()
{
    CameraContext cameraCtx = {MIN_X, MAX_X, MIN_Y, MAX_Y,
        START_X, START_Y, DEFAULT_ZOOM};
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Computing the Mandelbrot set");

    Image canvas = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    Texture2D fractalTexture = LoadTextureFromImage(canvas);

    while (!WindowShouldClose()) {
        UpdateCamera(&cameraCtx);

        UpdateImage(&canvas, &cameraCtx);        

        UpdateTexture(fractalTexture, canvas.data);

        BeginDrawing();
            DrawTexture(fractalTexture, 0, 0, WHITE);
            DrawFPS(10, 10);
        EndDrawing();

    }

    UnloadImage(canvas);
    UnloadTexture(fractalTexture);

    CloseWindow();
}


static void UpdateImage(Image* canvas, const CameraContext* cameraCtx)
{
    assert(canvas); assert(canvas->data); assert(cameraCtx);

    // #pragma omp parallel for schedule(dynamic)
    for (int posX = 0; posX < SCREEN_WIDTH; posX++) {
        for (int posY = 0; posY < SCREEN_HEIGHT; posY++) {
            ftype x0 = cameraCtx->minX + (ftype)posX / SCREEN_WIDTH * 
                (cameraCtx->maxX - cameraCtx->minX);
            ftype y0 = cameraCtx->minY + (ftype)posY / SCREEN_HEIGHT *
                (cameraCtx->maxY - cameraCtx->minY);

            int n = 0;

            for (ftype x = 0.0, y = 0.0; n < MAX_ITERATION_COUNT; n++) {
                ftype x_pow2 = x * x;
                ftype y_pow2 = y * y;
                ftype xy = x * y;

                if (x_pow2 + y_pow2 > NO_RETURN_POINT_POW2) {
                    break;
                }

                x = x_pow2 - y_pow2 + x0;
                y = 2 * xy + y0;
            }

            if (n == MAX_ITERATION_COUNT) {
                ImageDrawPixel(canvas, posX, posY, BLACK);
            } else {
                ImageDrawPixel(canvas, posX, posY, WHITE);
            }
        }
    }
}


static void UpdateCamera(CameraContext* cameraCtx)
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

