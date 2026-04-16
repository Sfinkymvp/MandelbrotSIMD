#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <assert.h>

#include "calc_package.h"


void UpdateImagePackageOptimization(Image* canvas, const CameraContext* cameraCtx)
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
#ifdef COLOR_DRAWING
                double smooth = n + 1.0 - log2(log2(sqrt(x * x + y * y)));
                finalColor = GetColor(fmodf((float)smooth * COLOR_CHANGE_COEFFICIENT, 1.0f));
#else
                finalColor = WHITE;
#endif // COLOR_DRAWING
                }
                pixels[posY * SCREEN_WIDTH + posX + i] = finalColor;
            }
        }
    }
}

