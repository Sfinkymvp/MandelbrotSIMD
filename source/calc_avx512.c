#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <immintrin.h>
#include <assert.h>

#include "calc_avx512.h"


const int DOUBLE_PACK_SIZE = 8;


void UpdateImageAVX512Optimization(Image* canvas, const CameraContext* cameraCtx)
{
    assert(canvas); assert(canvas->data); assert(cameraCtx);

    Color* pixels = (Color*)canvas->data;

// Включение многоядерной обработки программы
#ifdef MULTITHREAD_OPTIMIZATION
    #pragma omp parallel for schedule(dynamic)
#endif // MULTITHREAD_OPTIMIZATION

    for (int posY = 0; posY < SCREEN_HEIGHT; posY++) {
        for (int posX = 0; posX < SCREEN_WIDTH; posX += DOUBLE_PACK_SIZE) {
            double dx = (cameraCtx->maxX - cameraCtx->minX) / (double)SCREEN_WIDTH;
            __m512d index_vector = _mm512_set_pd(7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.0);
            __m512d arrayX0 = _mm512_set1_pd((double)posX);
            arrayX0 = _mm512_add_pd(arrayX0, index_vector);
            arrayX0 = _mm512_mul_pd(arrayX0, _mm512_set1_pd(dx));
            arrayX0 = _mm512_add_pd(arrayX0, _mm512_set1_pd(cameraCtx->minX));

            double dy = (cameraCtx->maxY - cameraCtx->minY) / (double)SCREEN_HEIGHT;
            __m512d arrayY0 = _mm512_set1_pd((double)posY);
            arrayY0 = _mm512_mul_pd(arrayY0, _mm512_set1_pd(dy));
            arrayY0 = _mm512_add_pd(arrayY0, _mm512_set1_pd(cameraCtx->minY));

            __m512d arrayX = _mm512_set1_pd(0.0);
            __m512d arrayY = _mm512_set1_pd(0.0);
            __m512d arrayXPow2 = _mm512_set1_pd(0.0);
            __m512d arrayYPow2 = _mm512_set1_pd(0.0);
            __m512d arrayModulusPow2 = _mm512_set1_pd(0.0);
            __m512i nCounter = _mm512_set1_epi64(0ll);

            __m512d two = _mm512_set1_pd(2.0);
            __m512i oneI = _mm512_set1_epi64(1ll);
            __m512d limitPow2 = _mm512_set1_pd(NO_RETURN_POINT_POW2);

            unsigned char alive_mask = 0xFF;
            int n = 0;

            for (; n < MAX_ITERATION_COUNT && alive_mask; n++) {
                arrayXPow2 = _mm512_mul_pd(arrayX, arrayX);
                arrayYPow2 = _mm512_mul_pd(arrayY, arrayY);

                arrayModulusPow2 = _mm512_add_pd(arrayXPow2, arrayYPow2);
                __mmask8 in_bounds = _mm512_cmp_pd_mask(arrayModulusPow2, limitPow2, _CMP_LT_OQ);
                alive_mask &= in_bounds;

                if (!alive_mask) {
                    break;
                }

                __m512d xy = _mm512_mul_pd(arrayX, arrayY);
                arrayY = _mm512_fmadd_pd(xy, two, arrayY0);
                arrayX = _mm512_sub_pd(arrayXPow2, arrayYPow2);
                arrayX = _mm512_add_pd(arrayX, arrayX0);

                nCounter = _mm512_mask_add_epi64(nCounter, alive_mask, nCounter, oneI);
            }

            long long resCounter[DOUBLE_PACK_SIZE] = {};
            double resModulusPow2[DOUBLE_PACK_SIZE] = {};
            _mm512_store_epi64(resCounter, nCounter);
            _mm512_store_pd(resModulusPow2, arrayModulusPow2);
            for (int i = 0; i < DOUBLE_PACK_SIZE; i++) {
                Color finalColor = {};
                if (resCounter[i] >= MAX_ITERATION_COUNT - 1) {
                    finalColor = BLACK;
                } else {
#ifdef COLOR_DRAWING
                int color = n % 2 * 255;
                finalColor = (Color){color, color, color, color};
#else
                finalColor = WHITE;
#endif // COLOR_DRAWING
                }

                pixels[posY * SCREEN_WIDTH + posX + i] = finalColor;
            }
        }
    }
}