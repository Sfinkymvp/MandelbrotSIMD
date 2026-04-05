#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <raylib.h>
#include <assert.h>

#include "calc.h"

#define AMAZING_COLORS {0.00f, 0.10f, 0.20f}
#define FIRE_COLORS {0.00f, 0.07f, 0.12f}
#define RAINBOW_COLORS {0.00f, 0.33f, 0.67f}
#define ICE_COLORS {0.30f, 0.20f, 0.20f}

// Если < 0.05, то цвета будут меняться плавно, если больше - резко
const float COLOR_CHANGE_COEFFICIENT = 0.05f;


static void UpdateImage(Image* canvas, const CameraContext* cameraCtx);
static void UpdateCamera(CameraContext* cameraCtx);
static void UpdateViewport(CameraContext* cameraCtx);
static Color GetPaletteColor(ftype t);

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

            ftype xPow2 = 0.0;
            ftype yPow2 = 0.0;
            for (ftype x = 0.0, y = 0.0; n < MAX_ITERATION_COUNT; n++) {
                xPow2 = x * x;
                yPow2 = y * y;
                ftype xy = x * y;

                if (xPow2 + yPow2 > NO_RETURN_POINT_POW2) {
                    break;
                }

                x = xPow2 - yPow2 + x0;
                y = 2 * xy + y0;
            }

            Color finalColor = {};
            if (n == MAX_ITERATION_COUNT) {
                finalColor = BLACK;
            } else {
                ftype modulus = sqrt(xPow2 + yPow2);
                ftype smooth = (ftype)n + 1.0 - log2(log2(modulus));

                float t = (float)smooth * COLOR_CHANGE_COEFFICIENT;
                finalColor = GetPaletteColor(fmodf(t, 1.0));
            }

            ImageDrawPixel(canvas, posX, posY, finalColor);
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


static Color GetPaletteColor(ftype t) {
    float a[] = {0.5f, 0.5f, 0.5f};
    float b[] = {0.5f, 0.5f, 0.5f};
    float c[] = {1.0f, 1.0f, 1.0f};
    float d[] = AMAZING_COLORS;

    unsigned char r = (unsigned char)(255 * (a[0] + b[0] * cos(2 * PI * (c[0] * t + d[0]))));
    unsigned char g = (unsigned char)(255 * (a[1] + b[1] * cos(2 * PI * (c[1] * t + d[1]))));
    unsigned char b_val = (unsigned char)(255 * (a[2] + b[2] * cos(2 * PI * (c[2] * t + d[2]))));

    return (Color){ r, g, b_val, 255 };
}