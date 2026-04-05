#include <stdio.h>
#include <raylib.h>

#include "calc.h"


typedef double ftype;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const ftype MIN_X = -2.0;
const ftype MAX_X = 1.0;
const ftype MIN_Y = -1.0;
const ftype MAX_Y = 1.0;

const int MAX_ITERATION_COUNT = 1000;
const int NO_RETURN_POINT_POW2 = 4.0;


void Function()
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Computing the Mandelbrot set");
    Image canvas = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    for (int posX = 0; posX < SCREEN_WIDTH; posX++) {
        for (int posY = 0; posY < SCREEN_HEIGHT; posY++) {
            ftype x0 = MIN_X + (ftype)posX / SCREEN_WIDTH * (MAX_X - MIN_X);
            ftype y0 = MIN_Y + (ftype)posY / SCREEN_HEIGHT * (MAX_Y - MIN_Y);

            ftype x = 0.0;
            ftype y = 0.0;
            int n = 0;

            for (; n < MAX_ITERATION_COUNT; n++) {
                ftype x_pow2 = x * x;
                ftype y_pow2 = y * y;
                ftype xy     = x * y;

                if (x_pow2 + y_pow2 > NO_RETURN_POINT_POW2) {
                    break;
                }

                x = x_pow2 - y_pow2 + x0;
                y = 2 * xy + y0;
            }

            if (n == MAX_ITERATION_COUNT) {
                ImageDrawPixel(&canvas, posX, posY, BLACK);
            } else {
                ImageDrawPixel(&canvas, posX, posY, WHITE);
            }
        }
    }

    Texture2D fractalTexture = LoadTextureFromImage(canvas);
    UnloadImage(canvas);


    while (!WindowShouldClose()) {
        BeginDrawing();
            DrawTexture(fractalTexture, 0, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();
}