/**
 * @file addons/script/functions_math.c
 * @brief Math functions for flecs script.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_MATH
#include "script.h"
#include <math.h>

typedef struct ecs_script_rng_t {
    uint64_t x; /* Current state (initialize with seed) */
    uint64_t w; /* Weyl sequence increment */
    uint64_t s; /* Constant for Weyl sequence */
    int32_t refcount; /* Necessary as flecs script doesn't have ref types */
    bool initialized;
} ecs_script_rng_t;

static
ecs_script_rng_t* flecs_script_rng_new(void) {
    ecs_script_rng_t *result = ecs_os_calloc_t(ecs_script_rng_t);
    result->x = 0;
    result->w = 0;
    result->s = 0xb5ad4eceda1ce2a9; /* Constant for the Weyl sequence */
    result->refcount = 1;
    result->initialized = false;
    return result;
}

static
void flecs_script_rng_keep(ecs_script_rng_t *rng) {
    if (!rng) {
        return;
    }
    rng->refcount ++;
}

static
void flecs_script_rng_free(ecs_script_rng_t *rng) {
    if (!rng) {
        return;
    }
    ecs_assert(rng->refcount > 0, ECS_INTERNAL_ERROR, NULL);
    if (!--rng->refcount) {
        ecs_os_free(rng);
    }
}

static
uint64_t flecs_script_rng_next(ecs_script_rng_t *rng) {
    rng->x *= rng->x;
    rng->x += (rng->w += rng->s);
    rng->x = (rng->x >> 32) | (rng->x << 32);
    return rng->x;
}

ECS_COMPONENT_DECLARE(EcsScriptRng);

static
ECS_CTOR(EcsScriptRng, ptr, {
    ptr->seed = 0;
    ptr->impl = flecs_script_rng_new();
})

static
ECS_COPY(EcsScriptRng, dst, src, {
    flecs_script_rng_keep(src->impl);
    if (dst->impl != src->impl) {
        flecs_script_rng_free(dst->impl);
    }
    dst->seed = src->seed;
    dst->impl = src->impl;
})

static
ECS_MOVE(EcsScriptRng, dst, src, {
    flecs_script_rng_free(dst->impl);
    dst->seed = src->seed;
    dst->impl = src->impl;
    src->impl = NULL;
})

static
ECS_DTOR(EcsScriptRng, ptr, {
    flecs_script_rng_free(ptr->impl);
})

void flecs_script_rng_get_float(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    EcsScriptRng *rng = argv[0].ptr;
    ecs_assert(rng->impl != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_rng_t *impl = rng->impl;
    if (!impl->initialized) {
        impl->x = rng->seed;
        impl->initialized = true;
    }
    uint64_t x = flecs_script_rng_next(rng->impl);
    double max = *(double*)argv[1].ptr;
    double *r = result->ptr;

    if (ECS_EQZERO(max)) {
        ecs_err("flecs.script.math.Rng.f(): invalid division by zero");
    } else {
        *r = (double)x / ((double)UINT64_MAX / max);
    }
}

void flecs_script_rng_get_uint(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    EcsScriptRng *rng = argv[0].ptr;
    ecs_assert(rng->impl != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_rng_t *impl = rng->impl;
    if (!impl->initialized) {
        impl->x = rng->seed;
        impl->initialized = true;
    }
    uint64_t x = flecs_script_rng_next(rng->impl);
    uint64_t max = *(uint64_t*)argv[1].ptr;
    uint64_t *r = result->ptr;
    if (!max) {
        ecs_err("flecs.script.math.Rng.u(): invalid division by zero");
    } else {
        *r = x % max;
    }
}

double flecs_lerp(
    double a, 
    double b,
    double t) 
{
    return a + t * (b - a);
}

void flecs_script_min(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    double a = *(double*)argv[0].ptr;
    double b = *(double*)argv[1].ptr;
    *(double*)result->ptr = (a < b) ? a : b;
}

void flecs_script_max(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    double a = *(double*)argv[0].ptr;
    double b = *(double*)argv[1].ptr;
    *(double*)result->ptr = (a > b) ? a : b;
}

#define FLECS_SCRIPT_LERP(type)\
    static void flecs_script_lerp_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        (void)ctx; (void)argc;\
        type *a = argv[0].ptr;\
        type *b = argv[1].ptr;\
        type t = (type)*(double*)argv[2].ptr;\
        type *r = result->ptr;\
        for (int i = 0; i < elem_count; i ++) {\
            r[i] = a[i] + t * (b[i] - a[i]);\
        }\
    }

FLECS_SCRIPT_LERP(float)
FLECS_SCRIPT_LERP(double)

#define FLECS_SCRIPT_SMOOTHSTEP(type)\
    void flecs_script_smoothstep_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        (void)ctx; (void)argc;\
        type *a = argv[0].ptr;\
        type *b = argv[1].ptr;\
        type t = (type)*(double*)argv[2].ptr;\
        type *r = result->ptr;\
        for (int i = 0; i < elem_count; i ++) {\
            type v = (t - a[i]) / (b[i] - a[i]);\
            type x = (v < (type)0) ? (type)0\
                   : (v > (type)1) ? (type)1\
                   : v;\
            r[i] = x * x * (3 - 2 * x);\
        }\
    }

FLECS_SCRIPT_SMOOTHSTEP(float)
FLECS_SCRIPT_SMOOTHSTEP(double)

#define FLECS_SCRIPT_CLAMP(type)\
    static void flecs_script_clamp_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        (void)ctx; (void)argc;\
        type *v = argv[0].ptr;\
        type min = (type)*(double*)argv[1].ptr;\
        type max = (type)*(double*)argv[2].ptr;\
        type *r = result->ptr;\
        for (int i = 0; i < elem_count; i ++) {\
            r[i] = (v[i] < min) ? min\
                 : (v[i] > max) ? max\
                 : v[i];\
        }\
    }

FLECS_SCRIPT_CLAMP(int8_t)
FLECS_SCRIPT_CLAMP(int16_t)
FLECS_SCRIPT_CLAMP(int32_t)
FLECS_SCRIPT_CLAMP(int64_t)
FLECS_SCRIPT_CLAMP(uint8_t)
FLECS_SCRIPT_CLAMP(uint16_t)
FLECS_SCRIPT_CLAMP(uint32_t)
FLECS_SCRIPT_CLAMP(uint64_t)
FLECS_SCRIPT_CLAMP(float)
FLECS_SCRIPT_CLAMP(double)

#define FLECS_SCRIPT_DOT(type)\
    static void flecs_script_dot_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        (void)ctx; (void)argc;\
        type *a = argv[0].ptr;\
        type *b = argv[1].ptr;\
        double *r = result->ptr;\
        for (int i = 0; i < elem_count; i ++) {\
            double x = a[i], y = b[i];\
            *r += x * y;\
        }\
    }

FLECS_SCRIPT_DOT(int8_t)
FLECS_SCRIPT_DOT(int16_t)
FLECS_SCRIPT_DOT(int32_t)
FLECS_SCRIPT_DOT(int64_t)
FLECS_SCRIPT_DOT(uint8_t)
FLECS_SCRIPT_DOT(uint16_t)
FLECS_SCRIPT_DOT(uint32_t)
FLECS_SCRIPT_DOT(uint64_t)
FLECS_SCRIPT_DOT(float)
FLECS_SCRIPT_DOT(double)

#define FLECS_SCRIPT_LENGTH(type)\
    static void flecs_script_length_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        (void)ctx; (void)argc;\
        type *v = argv[0].ptr;\
        double *r = result->ptr;\
        double sum = 0;\
        for (int i = 0; i < elem_count; i ++) {\
            double x = (double)v[i];\
            sum += x * x;\
        }\
        *r = sqrt(sum);\
    }

FLECS_SCRIPT_LENGTH(int8_t)
FLECS_SCRIPT_LENGTH(int16_t)
FLECS_SCRIPT_LENGTH(int32_t)
FLECS_SCRIPT_LENGTH(int64_t)
FLECS_SCRIPT_LENGTH(uint8_t)
FLECS_SCRIPT_LENGTH(uint16_t)
FLECS_SCRIPT_LENGTH(uint32_t)
FLECS_SCRIPT_LENGTH(uint64_t)
FLECS_SCRIPT_LENGTH(float)
FLECS_SCRIPT_LENGTH(double)

#define FLECS_SCRIPT_LENGTH_SQ(type)\
    static void flecs_script_length_sq_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        (void)ctx; (void)argc;\
        type *v = argv[0].ptr;\
        double *r = result->ptr;\
        for (int i = 0; i < elem_count; i ++) {\
            double x = (double)v[i];\
            *r += x * x;\
        }\
    }

FLECS_SCRIPT_LENGTH_SQ(int8_t)
FLECS_SCRIPT_LENGTH_SQ(int16_t)
FLECS_SCRIPT_LENGTH_SQ(int32_t)
FLECS_SCRIPT_LENGTH_SQ(int64_t)
FLECS_SCRIPT_LENGTH_SQ(uint8_t)
FLECS_SCRIPT_LENGTH_SQ(uint16_t)
FLECS_SCRIPT_LENGTH_SQ(uint32_t)
FLECS_SCRIPT_LENGTH_SQ(uint64_t)
FLECS_SCRIPT_LENGTH_SQ(float)
FLECS_SCRIPT_LENGTH_SQ(double)

#define FLECS_SCRIPT_NORMALIZE(type)\
    static void flecs_script_normalize_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        (void)ctx; (void)argc;\
        const type *v = argv[0].ptr;\
        type *r = result->ptr;\
        double sum_sq = 0.0;\
        for (int i = 0; i < elem_count; i++) {\
            double x = (double)v[i];\
            sum_sq += x * x;\
        }\
        if (sum_sq == 0.0) {\
            for (int i = 0; i < elem_count; i++) r[i] = (type)0;\
            return;\
        }\
        double inv_len = 1.0 / sqrt(sum_sq);\
        for (int i = 0; i < elem_count; i++) {\
            r[i] = (type)((double)v[i] * inv_len);\
        }\
    }

FLECS_SCRIPT_NORMALIZE(float)
FLECS_SCRIPT_NORMALIZE(double)

#define FLECS_MATH_FUNC_F64(name, ...)\
    static\
    void flecs_math_##name(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result)\
    {\
        (void)ctx; (void)argc;\
        ecs_assert(argc == 1, ECS_INTERNAL_ERROR, NULL);\
        double x = *(double*)argv[0].ptr;\
        *(double*)result->ptr = __VA_ARGS__;\
    }

#define FLECS_MATH_FUNC_F64_F64(name, ...)\
    static\
    void flecs_math_##name(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result)\
    {\
        (void)ctx; (void)argc;\
        ecs_assert(argc == 2, ECS_INTERNAL_ERROR, NULL);\
        double x = *(double*)argv[0].ptr;\
        double y = *(double*)argv[1].ptr;\
        *(double*)result->ptr = __VA_ARGS__;\
    }

#define FLECS_MATH_FUNC_F64_I32(name, ...)\
    static\
    void flecs_math_##name(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result)\
    {\
        (void)ctx; (void)argc;\
        ecs_assert(argc == 2, ECS_INTERNAL_ERROR, NULL);\
        double x = *(double*)argv[0].ptr;\
        ecs_i32_t y = *(ecs_i32_t*)argv[1].ptr;\
        *(double*)result->ptr = __VA_ARGS__;\
    }

#define FLECS_MATH_FUNC_DEF_F64(_name, brief)\
    {\
        ecs_entity_t f = ecs_function(world, {\
            .name = #_name,\
            .parent = ecs_id(FlecsScriptMath),\
            .return_type = ecs_id(ecs_f64_t),\
            .params = {{ .name = "x", .type = ecs_id(ecs_f64_t) }},\
            .callback = flecs_math_##_name\
        });\
        ecs_doc_set_brief(world, f, brief);\
    }

#define FLECS_MATH_FUNC_DEF_F64_F64(_name, brief)\
    {\
        ecs_entity_t f = ecs_function(world, {\
            .name = #_name,\
            .parent = ecs_id(FlecsScriptMath),\
            .return_type = ecs_id(ecs_f64_t),\
            .params = {\
                { .name = "x", .type = ecs_id(ecs_f64_t) },\
                { .name = "y", .type = ecs_id(ecs_f64_t) }\
            },\
            .callback = flecs_math_##_name\
        });\
        ecs_doc_set_brief(world, f, brief);\
    }

#define FLECS_MATH_FUNC_DEF_F64_F32(_name, brief)\
    {\
        ecs_entity_t f = ecs_function(world, {\
            .name = #_name,\
            .parent = ecs_id(FlecsScriptMath),\
            .return_type = ecs_id(ecs_f64_t),\
            .params = {\
                { .name = "x", .type = ecs_id(ecs_f64_t) },\
                { .name = "y", .type = ecs_id(ecs_i32_t) }\
            },\
            .callback = flecs_math_##_name\
        });\
        ecs_doc_set_brief(world, f, brief);\
    }

/* Trigonometric functions */
FLECS_MATH_FUNC_F64(cos, cos(x))
FLECS_MATH_FUNC_F64(sin, sin(x))
FLECS_MATH_FUNC_F64(tan, tan(x))
FLECS_MATH_FUNC_F64(acos, acos(x))
FLECS_MATH_FUNC_F64(asin, asin(x))
FLECS_MATH_FUNC_F64(atan, atan(x))
FLECS_MATH_FUNC_F64_F64(atan2, atan2(x, y))

/* Hyperbolic functions */
FLECS_MATH_FUNC_F64(cosh, cosh(x))
FLECS_MATH_FUNC_F64(sinh, sinh(x))
FLECS_MATH_FUNC_F64(tanh, tanh(x))
FLECS_MATH_FUNC_F64(acosh, acosh(x))
FLECS_MATH_FUNC_F64(asinh, asinh(x))
FLECS_MATH_FUNC_F64(atanh, atanh(x))

/* Exponential and logarithmic functions */
FLECS_MATH_FUNC_F64(exp, exp(x))
FLECS_MATH_FUNC_F64_I32(ldexp, ldexp(x, y))
FLECS_MATH_FUNC_F64(log, log(x))
FLECS_MATH_FUNC_F64(log10, log10(x))
FLECS_MATH_FUNC_F64(exp2, exp2(x))
FLECS_MATH_FUNC_F64(log2, log2(x))

/* Power functions */
FLECS_MATH_FUNC_F64_F64(pow, pow(x, y))
FLECS_MATH_FUNC_F64(sqrt, sqrt(x))
FLECS_MATH_FUNC_F64(sqr, x * x)

/* Rounding functions */
FLECS_MATH_FUNC_F64(ceil, ceil(x))
FLECS_MATH_FUNC_F64(floor, floor(x))
FLECS_MATH_FUNC_F64(round, round(x))

FLECS_MATH_FUNC_F64(abs, fabs(x))

void FlecsScriptMathImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsScriptMath);

    ECS_IMPORT(world, FlecsScript);

    /* Constants */
    double E = 2.71828182845904523536028747135266250;
    ecs_const_var(world, {
        .name = "E",
        .parent = ecs_id(FlecsScriptMath),
        .type = ecs_id(ecs_f64_t),
        .value = &E
    });

    double PI = 3.14159265358979323846264338327950288;
    ecs_const_var(world, {
        .name = "PI",
        .parent = ecs_id(FlecsScriptMath),
        .type = ecs_id(ecs_f64_t),
        .value = &PI
    });

    /* Trigonometric functions */
    FLECS_MATH_FUNC_DEF_F64(cos, "Compute cosine");
    FLECS_MATH_FUNC_DEF_F64(sin, "Compute sine");
    FLECS_MATH_FUNC_DEF_F64(tan, "Compute tangent");
    FLECS_MATH_FUNC_DEF_F64(acos, "Compute arc cosine");
    FLECS_MATH_FUNC_DEF_F64(asin, "Compute arc sine");
    FLECS_MATH_FUNC_DEF_F64(atan, "Compute arc tangent");
    FLECS_MATH_FUNC_DEF_F64_F64(atan2, "Compute arc tangent with two parameters");

    /* Hyperbolic functions */
    FLECS_MATH_FUNC_DEF_F64(cosh, "Compute hyperbolic cosine");
    FLECS_MATH_FUNC_DEF_F64(sinh, "Compute hyperbolic sine");
    FLECS_MATH_FUNC_DEF_F64(tanh, "Compute hyperbolic tangent");
    FLECS_MATH_FUNC_DEF_F64(acosh, "Compute area hyperbolic cosine");
    FLECS_MATH_FUNC_DEF_F64(asinh, "Compute area hyperbolic sine");
    FLECS_MATH_FUNC_DEF_F64(atanh, "Compute area hyperbolic tangent");

    /* Exponential and logarithmic functions */
    FLECS_MATH_FUNC_DEF_F64(exp, "Compute exponential function");
    FLECS_MATH_FUNC_DEF_F64_F32(ldexp, "Generate value from significant and exponent");
    FLECS_MATH_FUNC_DEF_F64(log, "Compute natural logarithm");
    FLECS_MATH_FUNC_DEF_F64(log10, "Compute common logarithm");
    FLECS_MATH_FUNC_DEF_F64(exp2, "Compute binary exponential function");
    FLECS_MATH_FUNC_DEF_F64(log2, "Compute binary logarithm");

    /* Power functions */
    FLECS_MATH_FUNC_DEF_F64_F64(pow, "Raise to power");
    FLECS_MATH_FUNC_DEF_F64(sqrt, "Compute square root");
    FLECS_MATH_FUNC_DEF_F64(sqr, "Compute square");

    /* Rounding functions */
    FLECS_MATH_FUNC_DEF_F64(ceil, "Round up value");
    FLECS_MATH_FUNC_DEF_F64(floor, "Round down value");
    FLECS_MATH_FUNC_DEF_F64(round, "Round to nearest");

    FLECS_MATH_FUNC_DEF_F64(abs, "Compute absolute value");

    ecs_set_name_prefix(world, "EcsScript");

    ECS_COMPONENT_DEFINE(world, EcsScriptRng);

    ecs_set_hooks(world, EcsScriptRng, {
        .ctor = ecs_ctor(EcsScriptRng),
        .move = ecs_move(EcsScriptRng),
        .copy = ecs_copy(EcsScriptRng),
        .dtor = ecs_dtor(EcsScriptRng),
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsScriptRng),
        .members = {
            { .name = "seed", .type = ecs_id(ecs_u64_t) }
        }
    });

    ecs_method(world, {
        .parent = ecs_id(EcsScriptRng),
        .name = "f",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "max", .type = ecs_id(ecs_f64_t) }
        },
        .callback = flecs_script_rng_get_float
    });

    ecs_method(world, {
        .parent = ecs_id(EcsScriptRng),
        .name = "u",
        .return_type = ecs_id(ecs_u64_t),
        .params = {
            { .name = "max", .type = ecs_id(ecs_u64_t) }
        },
        .callback = flecs_script_rng_get_uint
    });

    ecs_function(world, {
        .name = "min",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "a", .type = ecs_id(ecs_f64_t) },
            { .name = "b", .type = ecs_id(ecs_f64_t) },
        },
        .callback = flecs_script_min
    });

    ecs_function(world, {
        .name = "max",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "a", .type = ecs_id(ecs_f64_t) },
            { .name = "b", .type = ecs_id(ecs_f64_t) },
        },
        .callback = flecs_script_max
    });

    ecs_function(world, {
        .name = "dot",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "a", .type = EcsScriptVectorType },
            { .name = "b", .type = EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI8]  = flecs_script_dot_int8_t,
            [EcsI16] = flecs_script_dot_int16_t,
            [EcsI32] = flecs_script_dot_int32_t,
            [EcsI64] = flecs_script_dot_int64_t,
            [EcsU8]  = flecs_script_dot_uint8_t,
            [EcsU16] = flecs_script_dot_uint16_t,
            [EcsU32] = flecs_script_dot_uint32_t,
            [EcsU64] = flecs_script_dot_uint64_t,
            [EcsF32] = flecs_script_dot_float,
            [EcsF64] = flecs_script_dot_double
        }
    });

    ecs_function(world, {
        .name = "length",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "v", .type = EcsScriptVectorType }
        },
        .vector_callbacks = {
            [EcsI8]  = flecs_script_length_int8_t,
            [EcsI16] = flecs_script_length_int16_t,
            [EcsI32] = flecs_script_length_int32_t,
            [EcsI64] = flecs_script_length_int64_t,
            [EcsU8]  = flecs_script_length_uint8_t,
            [EcsU16] = flecs_script_length_uint16_t,
            [EcsU32] = flecs_script_length_uint32_t,
            [EcsU64] = flecs_script_length_uint64_t,
            [EcsF32] = flecs_script_length_float,
            [EcsF64] = flecs_script_length_double
        }
    });

    ecs_function(world, {
        .name = "length_sq",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "v", .type = EcsScriptVectorType }
        },
        .vector_callbacks = {
            [EcsI8]  = flecs_script_length_sq_int8_t,
            [EcsI16] = flecs_script_length_sq_int16_t,
            [EcsI32] = flecs_script_length_sq_int32_t,
            [EcsI64] = flecs_script_length_sq_int64_t,
            [EcsU8]  = flecs_script_length_sq_uint8_t,
            [EcsU16] = flecs_script_length_sq_uint16_t,
            [EcsU32] = flecs_script_length_sq_uint32_t,
            [EcsU64] = flecs_script_length_sq_uint64_t,
            [EcsF32] = flecs_script_length_sq_float,
            [EcsF64] = flecs_script_length_sq_double
        }
    });

    ecs_function(world, {
        .name = "normalize",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = EcsScriptVectorType,
        .params = {
            { .name = "v", .type = EcsScriptVectorType }
        },
        .vector_callbacks = {
            [EcsF32] = flecs_script_normalize_float,
            [EcsF64] = flecs_script_normalize_double
        }
    });

    ecs_function(world, {
        .name = "lerp",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = EcsScriptVectorType,
        .params = {
            { .name = "a", .type = EcsScriptVectorType },
            { .name = "b", .type = EcsScriptVectorType },
            { .name = "t", .type = ecs_id(ecs_f64_t) }
        },
        .vector_callbacks = {
            [EcsF32] = flecs_script_lerp_float,
            [EcsF64] = flecs_script_lerp_double
        }
    });

    ecs_function(world, {
        .name = "smoothstep",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = EcsScriptVectorType,
        .params = {
            { .name = "a", .type = EcsScriptVectorType },
            { .name = "b", .type = EcsScriptVectorType },
            { .name = "t", .type = ecs_id(ecs_f64_t) }
        },
        .vector_callbacks = {
            [EcsF32] = flecs_script_smoothstep_float,
            [EcsF64] = flecs_script_smoothstep_double
        }
    });

    ecs_function(world, {
        .name = "clamp",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = EcsScriptVectorType,
        .params = {
            { .name = "v", .type = EcsScriptVectorType },
            { .name = "min", .type = ecs_id(ecs_f64_t) },
            { .name = "max", .type = ecs_id(ecs_f64_t) }
        },
        .vector_callbacks = {
            [EcsI8] = flecs_script_clamp_int8_t,
            [EcsI16] = flecs_script_clamp_int16_t,
            [EcsI32] = flecs_script_clamp_int32_t,
            [EcsI64] = flecs_script_clamp_int64_t,
            [EcsU8] = flecs_script_clamp_uint8_t,
            [EcsU16] = flecs_script_clamp_uint16_t,
            [EcsU32] = flecs_script_clamp_uint32_t,
            [EcsU64] = flecs_script_clamp_uint64_t,
            [EcsF32] = flecs_script_clamp_float,
            [EcsF64] = flecs_script_clamp_double
        }
    });

    FlecsScriptMathPerlinImport(world);
}

#endif
