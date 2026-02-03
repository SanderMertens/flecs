/**
 * @file addons/script/functions_math_perlin.c
 * @brief Perlin noise functions for flecs script.
 * 
 * - flecs_perlin2(x, y): returns noise in ~[-1, 1]
 * - flecs_perlin_seed(seed): initializes permutation table for repeatable noise
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_MATH
#include "script.h"
#include <math.h>

static uint8_t flecs_perlin_perm[512];

static
uint32_t flecs_perlin_xs32(
    uint32_t *state) 
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

void flecs_perlin_seed(
    uint32_t seed) 
{
    uint8_t p[256];
    for (int i = 0; i < 256; i++) p[i] = (uint8_t)i;

    uint32_t st = (seed ? seed : 0xA341316Cu);

    for (int i = 255; i > 0; i--) {
        uint32_t r = flecs_perlin_xs32(&st);
        int j = (int)(r % (uint32_t)(i + 1));
        uint8_t tmp = p[i];
        p[i] = p[j];
        p[j] = tmp;
    }

    for (int i = 0; i < 256; i++) {
        flecs_perlin_perm[i] = p[i];
        flecs_perlin_perm[i + 256] = p[i];
    }
}

static 
double flecs_perlin_fade(
    double t) 
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

static 
double flecs_perlin_grad2(
    int hash, 
    double x, 
    double y) 
{
    switch (hash & 7) {
        case 0: return  x + y;
        case 1: return -x + y;
        case 2: return  x - y;
        case 3: return -x - y;
        case 4: return  x;
        case 5: return -x;
        case 6: return  y;
        default: return -y;
    }
}

static
double flecs_perlin2(
    double x, 
    double y) 
{
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    double xf = x - floor(x);
    double yf = y - floor(y);

    double u = flecs_perlin_fade(xf);
    double v = flecs_perlin_fade(yf);

    int aa = flecs_perlin_perm[flecs_perlin_perm[X] + Y];
    int ab = flecs_perlin_perm[flecs_perlin_perm[X] + Y + 1];
    int ba = flecs_perlin_perm[flecs_perlin_perm[X + 1] + Y];
    int bb = flecs_perlin_perm[flecs_perlin_perm[X + 1] + Y + 1];

    double x1 = flecs_lerp(
        flecs_perlin_grad2(aa, xf, yf),
        flecs_perlin_grad2(ba, xf - 1, yf),
        u);

    double x2 = flecs_lerp(
        flecs_perlin_grad2(ab, xf, yf - 1), 
        flecs_perlin_grad2(bb, xf - 1, yf - 1), 
        u);

    return flecs_lerp(x1, x2, v);
}

static
void flecs_script_perlin2(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    (void)argc;
    (void)ctx;
    double x = *(double*)argv[0].ptr;
    double y = *(double*)argv[1].ptr;
    *(double*)result->ptr = flecs_perlin2(x, y);
}

void FlecsScriptMathPerlinImport(
    ecs_world_t *world)
{
    ecs_entity_t FlecsScriptMath = ecs_lookup(world, "flecs.script.math");
    ecs_assert(FlecsScriptMath != 0, ECS_INTERNAL_ERROR, NULL);

    flecs_perlin_seed(1234);

    ecs_function(world, {
        .name = "perlin2",
        .parent = FlecsScriptMath,
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "x", .type = ecs_id(ecs_f64_t) },
            { .name = "y", .type = ecs_id(ecs_f64_t) },
        },
        .callback = flecs_script_perlin2
    });
}

#endif
