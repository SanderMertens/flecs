/**
 * @file addons/script/modules/functions_math.c
 * @brief Math functions for flecs script.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_MATH
#include "../script.h"
#include <math.h>

typedef struct ecs_script_rng_t {
    uint64_t x; /* Current state (initialize with seed) */
    uint64_t w; /* Weyl sequence increment */
    uint64_t s; /* Constant for Weyl sequence */
    int32_t refcount; /* Necessary as flecs script doesn't have ref types */
    bool initialized;
} ecs_script_rng_t;

static ecs_script_rng_t* flecs_script_rng_new(void) {
    ecs_script_rng_t *result = ecs_os_calloc_t(ecs_script_rng_t);
    result->x = 0;
    result->w = 0;
    result->s = 0xb5ad4eceda1ce2a9; /* Constant for the Weyl sequence */
    result->refcount = 1;
    result->initialized = false;
    return result;
}

static void flecs_script_rng_keep(ecs_script_rng_t *rng) {
    if (!rng) {
        return;
    }

    rng->refcount ++;
}

static void flecs_script_rng_free(ecs_script_rng_t *rng) {
    if (!rng) {
        return;
    }

    ecs_assert(rng->refcount > 0, ECS_INTERNAL_ERROR, NULL);
    if (!--rng->refcount) {
        ecs_os_free(rng);
    }
}

static uint64_t flecs_script_rng_mix(uint64_t x) {
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
}

static void flecs_script_rng_init(ecs_script_rng_t *rng, uint64_t seed) {
    if (rng->initialized) {
        return;
    }

    rng->x = flecs_script_rng_mix(seed);
    rng->w = flecs_script_rng_mix(rng->x);
    rng->initialized = true;
}

static uint64_t flecs_script_rng_next(ecs_script_rng_t *rng) {
    rng->x *= rng->x;
    rng->x += (rng->w += rng->s);
    rng->x = (rng->x >> 32) | (rng->x << 32);
    return rng->x;
}

ECS_COMPONENT_DECLARE(EcsScriptRng);

static ECS_CTOR(EcsScriptRng, ptr, {
    ptr->seed = 0;
    ptr->impl = flecs_script_rng_new();
})

static ECS_COPY(EcsScriptRng, dst, src, {
    flecs_script_rng_keep(src->impl);
    if (dst->impl != src->impl) {
        flecs_script_rng_free(dst->impl);
    }

    dst->seed = src->seed;
    dst->impl = src->impl;
})

static ECS_MOVE(EcsScriptRng, dst, src, {
    flecs_script_rng_free(dst->impl);
    dst->seed = src->seed;
    dst->impl = src->impl;
    src->impl = NULL;
})

static ECS_DTOR(EcsScriptRng, ptr, {
    flecs_script_rng_free(ptr->impl);
})

static void flecs_script_rng_get_float(
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
    flecs_script_rng_init(impl, rng->seed);
    uint64_t x = flecs_script_rng_next(rng->impl);
    double max = *(double*)argv[1].ptr;
    double *r = result->ptr;

    if (ECS_EQZERO(max)) {
        ecs_err("flecs.script.math.Rng.f(): invalid division by zero");
        flecs_script_runtime_get(ctx->world)->error = true;
    } else {
        *r = (double)x / ((double)UINT64_MAX / max);
    }
}

static void flecs_script_rng_get_uint(
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
    flecs_script_rng_init(impl, rng->seed);
    uint64_t x = flecs_script_rng_next(rng->impl);
    uint64_t max = *(uint64_t*)argv[1].ptr;
    uint64_t *r = result->ptr;
    if (!max) {
        ecs_err("flecs.script.math.Rng.u(): invalid division by zero");
        flecs_script_runtime_get(ctx->world)->error = true;
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

static void flecs_script_min(
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

static void flecs_script_max(
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

#define FLECS_SCRIPT_FOR_EACH_NUMERIC(M)\
    M(int8_t) M(int16_t) M(int32_t) M(int64_t)\
    M(uint8_t) M(uint16_t) M(uint32_t) M(uint64_t)\
    M(float) M(double)

#define FLECS_SCRIPT_NUMERIC_CALLBACKS(fn) {\
    [EcsI8] = fn##_int8_t, [EcsI16] = fn##_int16_t,\
    [EcsI32] = fn##_int32_t, [EcsI64] = fn##_int64_t,\
    [EcsU8] = fn##_uint8_t, [EcsU16] = fn##_uint16_t,\
    [EcsU32] = fn##_uint32_t, [EcsU64] = fn##_uint64_t,\
    [EcsF32] = fn##_float, [EcsF64] = fn##_double }

#define FLECS_SCRIPT_FLOAT_CALLBACKS(fn) {\
    [EcsF32] = fn##_float, [EcsF64] = fn##_double }

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
    static void flecs_script_smoothstep_##type(\
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

FLECS_SCRIPT_FOR_EACH_NUMERIC(FLECS_SCRIPT_CLAMP)

typedef struct flecs_math_function_t {
    const char *name;
    const char *brief;
    double (*unary)(double);
    double (*binary)(double, double);
    double (*exponent)(double, int);
} flecs_math_function_t;

#define FLECS_MATH_UNARY(name, expr)\
    static double flecs_math_##name(\
        double x)\
    {\
        return expr;\
    }

FLECS_MATH_UNARY(sqrt, sqrt(x))
FLECS_MATH_UNARY(sqr, x * x)
FLECS_MATH_UNARY(ceil, ceil(x))
FLECS_MATH_UNARY(floor, floor(x))
FLECS_MATH_UNARY(round, round(x))
FLECS_MATH_UNARY(abs, fabs(x))
FLECS_MATH_UNARY(log2, log2(x))

static const flecs_math_function_t flecs_math_length = { .unary = flecs_math_sqrt };

#define FLECS_SCRIPT_DOT(type)\
    static void flecs_script_dot_##type(\
        const ecs_function_ctx_t *ctx,\
        int32_t argc,\
        const ecs_value_t *argv,\
        ecs_value_t *result,\
        int32_t elem_count)\
    {\
        const type *a = argv[0].ptr;\
        const type *b = argv[argc - 1].ptr;\
        double sum = 0;\
        for (int i = 0; i < elem_count; i ++) {\
            sum += (double)a[i] * (double)b[i];\
        }\
        const flecs_math_function_t *fn = ctx->ctx;\
        *(double*)result->ptr = fn ? fn->unary(sum) : sum;\
    }

FLECS_SCRIPT_FOR_EACH_NUMERIC(FLECS_SCRIPT_DOT)

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
        if (ECS_EQZERO(sum_sq)) {\
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


static const flecs_math_function_t flecs_math_functions[] = {
    {"cos", "Compute cosine", .unary = cos},
    {"sin", "Compute sine", .unary = sin},
    {"tan", "Compute tangent", .unary = tan},
    {"acos", "Compute arc cosine", .unary = acos},
    {"asin", "Compute arc sine", .unary = asin},
    {"atan", "Compute arc tangent", .unary = atan},
    {"atan2", "Compute arc tangent with two parameters", .binary = atan2},
    {"cosh", "Compute hyperbolic cosine", .unary = cosh},
    {"sinh", "Compute hyperbolic sine", .unary = sinh},
    {"tanh", "Compute hyperbolic tangent", .unary = tanh},
    {"acosh", "Compute area hyperbolic cosine", .unary = acosh},
    {"asinh", "Compute area hyperbolic sine", .unary = asinh},
    {"atanh", "Compute area hyperbolic tangent", .unary = atanh},
    {"exp", "Compute exponential function", .unary = exp},
    {"ldexp", "Generate value from significand and exponent", .exponent = ldexp},
    {"log", "Compute natural logarithm", .unary = log},
    {"log10", "Compute common logarithm", .unary = log10},
    {"exp2", "Compute binary exponential function", .unary = exp2},
    {"log2", "Compute binary logarithm", .unary = flecs_math_log2},
    {"pow", "Raise to power", .binary = pow},
    {"sqrt", "Compute square root", .unary = flecs_math_sqrt},
    {"sqr", "Compute square", .unary = flecs_math_sqr},
    {"ceil", "Round up value", .unary = flecs_math_ceil},
    {"floor", "Round down value", .unary = flecs_math_floor},
    {"round", "Round to nearest", .unary = flecs_math_round},
    {"abs", "Compute absolute value", .unary = flecs_math_abs},
};

static void flecs_math_call(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    const flecs_math_function_t *fn = ctx->ctx;
    double x = *(double*)argv[0].ptr;
    if (fn->unary) {
        *(double*)result->ptr = fn->unary(x);
    } else if (fn->binary) {
        *(double*)result->ptr = fn->binary(x, *(double*)argv[1].ptr);
    } else {
        *(double*)result->ptr = fn->exponent(x, *(int32_t*)argv[1].ptr);
    }
}

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

    for (int32_t i = 0; i < (int32_t)(sizeof(flecs_math_functions) /
        sizeof(flecs_math_functions[0])); i ++)
    {
        const flecs_math_function_t *fn = &flecs_math_functions[i];
        ecs_function_desc_t desc = {
            .name = fn->name,
            .parent = ecs_id(FlecsScriptMath),
            .return_type = ecs_id(ecs_f64_t),
            .params = {{ .name = "x", .type = ecs_id(ecs_f64_t) }},
            .callback = flecs_math_call,
            .ctx = ECS_CONST_CAST(flecs_math_function_t*, fn)
        };
        if (fn->binary || fn->exponent) {
            desc.params[1].name = "y";
            desc.params[1].type = fn->exponent
                ? ecs_id(ecs_i32_t) : ecs_id(ecs_f64_t);
        }

        ecs_entity_t f = ecs_function_init(world, &desc);
        ecs_doc_set_brief(world, f, fn->brief);
    }

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
        .vector_callbacks = FLECS_SCRIPT_NUMERIC_CALLBACKS(flecs_script_dot)
    });

    ecs_function(world, {
        .name = "length",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "v", .type = EcsScriptVectorType }
        },
        .vector_callbacks = FLECS_SCRIPT_NUMERIC_CALLBACKS(flecs_script_dot),
        .ctx = ECS_CONST_CAST(flecs_math_function_t*, &flecs_math_length)
    });

    ecs_function(world, {
        .name = "length_sq",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { .name = "v", .type = EcsScriptVectorType }
        },
        .vector_callbacks = FLECS_SCRIPT_NUMERIC_CALLBACKS(flecs_script_dot)
    });

    ecs_function(world, {
        .name = "normalize",
        .parent = ecs_id(FlecsScriptMath),
        .return_type = EcsScriptVectorType,
        .params = {
            { .name = "v", .type = EcsScriptVectorType }
        },
        .vector_callbacks = FLECS_SCRIPT_FLOAT_CALLBACKS(flecs_script_normalize)
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
        .vector_callbacks = FLECS_SCRIPT_FLOAT_CALLBACKS(flecs_script_lerp)
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
        .vector_callbacks = FLECS_SCRIPT_FLOAT_CALLBACKS(flecs_script_smoothstep)
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
        .vector_callbacks = FLECS_SCRIPT_NUMERIC_CALLBACKS(flecs_script_clamp)
    });

    FlecsScriptMathPerlinImport(world);
}

#endif
