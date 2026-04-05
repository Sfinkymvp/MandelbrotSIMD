#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <raylib.h>
#include <immintrin.h>
#include <assert.h>

#include "calc.h"

// Цветовые схемы для множества Мандельброта
#define AMAZING_COLORS {0.00f, 0.10f, 0.20f}
#define FIRE_COLORS {0.00f, 0.07f, 0.12f}
#define RAINBOW_COLORS {0.00f, 0.33f, 0.67f}
#define ICE_COLORS {0.30f, 0.20f, 0.20f}

// Если < 0.05, то цвета будут меняться плавно, если больше - резко
const float COLOR_CHANGE_COEFFICIENT = 0.05f;


static void UpdateImageNoOptimization(Image* canvas, const CameraContext* cameraCtx);
static void UpdateImagePackageOptimization(Image* canvas, const CameraContext* cameraCtx);
static void UpdateCamera(CameraContext* cameraCtx);
static void UpdateViewport(CameraContext* cameraCtx);
static Color GetColor(ftype t);


void DrawFractal()
{
    CameraContext cameraCtx = {MIN_X, MAX_X, MIN_Y, MAX_Y,
        START_X, START_Y, DEFAULT_ZOOM};
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Computing the Mandelbrot set");

    Image canvas = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    Texture2D fractalTexture = LoadTextureFromImage(canvas);

    while (!WindowShouldClose()) {
        UpdateCamera(&cameraCtx);

        UpdateImagePackageOptimization(&canvas, &cameraCtx);        
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


static void UpdateImageNoOptimization(Image* canvas, const CameraContext* cameraCtx)
{
    assert(canvas); assert(canvas->data); assert(cameraCtx);

    // Обращаемся напрямую к памяти для доступа к пикселям
    Color* pixels = (Color*)canvas->data;

    for (int posY = 0; posY < SCREEN_HEIGHT; posY++) {
        for (int posX = 0; posX < SCREEN_WIDTH; posX++) {
            
            double x0 = cameraCtx->minX + (double)posX / SCREEN_WIDTH * (cameraCtx->maxX - cameraCtx->minX);
            double y0 = cameraCtx->minY + (double)posY / SCREEN_HEIGHT * (cameraCtx->maxY - cameraCtx->minY);

            int n = 0;

            double x = 0.0;
            double y = 0.0;
            for (; n < MAX_ITERATION_COUNT; n++) {
                double x2 = x * x;
                double y2 = y * y;
                
                y = 2.0 * x * y + y0;
                x = x2 - y2 + x0;
               
                if (x2 + y2 > NO_RETURN_POINT_POW2) {
                    break;
                } 
            }

            for (int i = 0; i < DOUBLE_PACK_SIZE; i++) {
                Color finalColor = {};

                if (n >= MAX_ITERATION_COUNT - 1) {
                    finalColor = BLACK;
                } else {
                    double smooth = n + 1.0 - log2(log2(sqrt(x * x + y * y)));
                    finalColor = GetColor(fmodf((float)smooth * COLOR_CHANGE_COEFFICIENT, 1.0f));
                }

                pixels[posY * SCREEN_WIDTH + posX] = finalColor;
            }
        }
    }
}

static void UpdateImagePackageOptimization(Image* canvas, const CameraContext* cameraCtx)
{
    assert(canvas); assert(canvas->data); assert(cameraCtx);

    Color* pixels = (Color*)canvas->data;

    for (int posY = 0; posY < SCREEN_HEIGHT; posY++) {
        for (int posX = 0; posX < SCREEN_WIDTH; posX += DOUBLE_PACK_SIZE) {
            
            double arrayX0[DOUBLE_PACK_SIZE] = {};
            double arrayY0[DOUBLE_PACK_SIZE] = {};
            double arrayX[DOUBLE_PACK_SIZE] = {};
            double arrayY[DOUBLE_PACK_SIZE] = {};
            double arrayN[DOUBLE_PACK_SIZE] = {};
            double arrayModulusSq[DOUBLE_PACK_SIZE] = {};

            for (int i = 0; i < DOUBLE_PACK_SIZE; i++) {
                arrayX0[i] = cameraCtx->minX + (double)(posX + i) / SCREEN_WIDTH * (cameraCtx->maxX - cameraCtx->minX);
                arrayY0[i] = cameraCtx->minY + (double)posY / SCREEN_HEIGHT * (cameraCtx->maxY - cameraCtx->minY);
            }

            unsigned char alive_mask = 0xFF;
            int n = 0;

            for (; n < MAX_ITERATION_COUNT && alive_mask; n++) {
                for (int i = 0; i < DOUBLE_PACK_SIZE; i++) {
                    bool is_alive = (alive_mask & (1u << i));
                    
                    if (is_alive) {
                        double x2 = arrayX[i] * arrayX[i];
                        double y2 = arrayY[i] * arrayY[i];
                        
                        arrayY[i] = 2.0 * arrayX[i] * arrayY[i] + arrayY0[i];
                        arrayX[i] = x2 - y2 + arrayX0[i];
                        
                        double m2 = x2 + y2;
                        if (m2 > NO_RETURN_POINT_POW2) {
                            alive_mask &= (unsigned char)~(1u << i);
                            arrayModulusSq[i] = m2;
                        } else {
                            arrayN[i]++;
                        }
                    }
                }
            }

            for (int i = 0; i < DOUBLE_PACK_SIZE; i++) {
                Color finalColor = {};

                if (arrayN[i] >= MAX_ITERATION_COUNT - 1) {
                    finalColor = BLACK;
                } else {
                    double smooth = arrayN[i] + 1.0 - log2(log2(sqrt(arrayModulusSq[i])));
                    finalColor = GetColor(fmodf((float)smooth * COLOR_CHANGE_COEFFICIENT, 1.0f));
                }
                pixels[posY * SCREEN_WIDTH + posX + i] = finalColor;
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


static Color GetColor(ftype t) {
    float a[] = {0.5f, 0.5f, 0.5f};
    float b[] = {0.5f, 0.5f, 0.5f};
    float c[] = {1.0f, 1.0f, 1.0f};
    float d[] = AMAZING_COLORS;

    unsigned char r = (unsigned char)(255 * (a[0] + b[0] * cos(2 * PI * (c[0] * t + d[0]))));
    unsigned char g = (unsigned char)(255 * (a[1] + b[1] * cos(2 * PI * (c[1] * t + d[1]))));
    unsigned char b_val = (unsigned char)(255 * (a[2] + b[2] * cos(2 * PI * (c[2] * t + d[2]))));

    return (Color){ r, g, b_val, 255 };
}