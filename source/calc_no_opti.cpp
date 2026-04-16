#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <assert.h>

#include "calc_no_opti.h"


void UpdateImageNoOptimization(Image* canvas, const CameraContext* cameraCtx)
{
    assert(canvas); assert(canvas->data); assert(cameraCtx);

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

            Color finalColor = {};

            if (n >= MAX_ITERATION_COUNT - 1) {
                finalColor = BLACK;
            } else {
#ifdef COLOR_DRAWING
                double smooth = n + 1.0 - log2(log2(sqrt(x * x + y * y)));
                finalColor = GetColor(fmodf((float)smooth * COLOR_CHANGE_COEFFICIENT, 1.0f));
#else
                finalColor = WHITE;
#endif // COLOR_DRAWING
            }

            pixels[posY * SCREEN_WIDTH + posX] = finalColor;
        }
    }
}

