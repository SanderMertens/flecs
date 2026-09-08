#include <script_math.h>
#include <stdio.h>

#define RNG_SEED_COUNT (256)

static ecs_entity_t rng_test_value_type(
    ecs_world_t *world)
{
    return ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Value" }),
        .members = {
            { .name = "v", .type = ecs_id(ecs_u64_t) }
        }
    });
}

static uint64_t rng_first_draw(
    ecs_world_t *world,
    ecs_entity_t value_id,
    uint64_t seed,
    uint64_t max)
{
    char expr[256];
    ecs_os_snprintf(expr, sizeof(expr),
        "using flecs.script.math\n"
        "const rng: Rng = {seed: %u}\n"
        "e { Value: {$rng.u(%u)} }\n",
        (unsigned int)seed, (unsigned int)max);

    assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    assert(e != 0);

    const uint64_t *ptr = ecs_get_id(world, e, value_id);
    assert(ptr != NULL);

    return *ptr;
}

static void test_rng_first_draw_uniform(
    ecs_world_t *world,
    ecs_entity_t value_id,
    uint64_t max)
{
    int32_t buckets[64] = {0};
    int32_t i;

    for (i = 0; i < RNG_SEED_COUNT; i ++) {
        uint64_t draw = rng_first_draw(world, value_id, (uint64_t)i, max);
        assert(draw < max);
        buckets[draw] ++;
    }

    double expected = (double)RNG_SEED_COUNT / (double)max;

    for (i = 0; i < (int32_t)max; i ++) {
        if ((double)buckets[i] < (expected * 0.4)) {
            printf("rng: bucket %d of u(%u) got %d draws, expected ~%.1f\n",
                i, (unsigned int)max, buckets[i], expected);
            assert(false);
        }
    }
}

static void test_rng_adjacent_seeds_differ(
    ecs_world_t *world,
    ecs_entity_t value_id)
{
    int32_t identical = 0;
    uint64_t prev = 0;
    int32_t i;

    for (i = 0; i < RNG_SEED_COUNT; i ++) {
        uint64_t draw = rng_first_draw(world, value_id, (uint64_t)i, 6);
        if (i && (draw == prev)) {
            identical ++;
        }
        prev = draw;
    }

    if (identical >= (((RNG_SEED_COUNT - 1) * 3) / 10)) {
        printf("rng: %d of %d adjacent seeds share their first u(6) draw\n",
            identical, RNG_SEED_COUNT - 1);
        assert(false);
    }
}

static void test_rng_same_seed_same_sequence(
    ecs_world_t *world,
    ecs_entity_t value_id)
{
    assert(ecs_script_run(world, NULL,
        "using flecs.script.math\n"
        "const a: Rng = {seed: 42}\n"
        "const b: Rng = {seed: 42}\n"
        "a0 { Value: {$a.u(1000000)} }\n"
        "a1 { Value: {$a.u(1000000)} }\n"
        "a2 { Value: {$a.u(1000000)} }\n"
        "a3 { Value: {$a.u(1000000)} }\n"
        "b0 { Value: {$b.u(1000000)} }\n"
        "b1 { Value: {$b.u(1000000)} }\n"
        "b2 { Value: {$b.u(1000000)} }\n"
        "b3 { Value: {$b.u(1000000)} }\n", NULL) == 0);

    const char *a_names[] = {"a0", "a1", "a2", "a3"};
    const char *b_names[] = {"b0", "b1", "b2", "b3"};
    uint64_t a_values[4];
    int32_t i;

    for (i = 0; i < 4; i ++) {
        ecs_entity_t a = ecs_lookup(world, a_names[i]);
        ecs_entity_t b = ecs_lookup(world, b_names[i]);
        assert(a != 0);
        assert(b != 0);

        const uint64_t *a_ptr = ecs_get_id(world, a, value_id);
        const uint64_t *b_ptr = ecs_get_id(world, b, value_id);
        assert(a_ptr != NULL);
        assert(b_ptr != NULL);
        assert(*a_ptr == *b_ptr);

        a_values[i] = *a_ptr;
    }

    assert(a_values[0] != a_values[1] || a_values[1] != a_values[2] ||
        a_values[2] != a_values[3]);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsScriptMath);

    int32_t v = 0;
    (void)v;

    assert(ecs_expr_run(world, "flecs.script.math.sqr(10)", 
        &ecs_value_ptr(ecs_i32_t, &v), NULL) != NULL);
    assert(v == 100);

    assert(ecs_expr_run(world, "flecs.script.math.sqrt(100)", 
        &ecs_value_ptr(ecs_i32_t, &v), NULL) != NULL);
    assert(v == 10);

    assert(ecs_expr_run(world, "flecs.script.math.pow(5, 2)", 
        &ecs_value_ptr(ecs_i32_t, &v), NULL) != NULL);
    assert(v == 25);

    assert(ecs_expr_run(world, "flecs.script.math.ceil(1.6)", 
        &ecs_value_ptr(ecs_i32_t, &v), NULL) != NULL);
    assert(v == 2);

    assert(ecs_expr_run(world, "flecs.script.math.floor(1.6)", 
        &ecs_value_ptr(ecs_i32_t, &v), NULL) != NULL);
    assert(v == 1);

    assert(ecs_expr_run(world, "flecs.script.math.round(1.6)", 
        &ecs_value_ptr(ecs_i32_t, &v), NULL) != NULL);
    assert(v == 2);

    assert(ecs_script_run(world, NULL,
        "using flecs.script.math\n"
        "const rng: Rng = {seed: 42}\n"
        "const x = $rng.f(10.0)\n", NULL) == 0);

    ecs_log_set_level(-4);
    assert(ecs_script_run(world, NULL,
        "using flecs.script.math\n"
        "const rng: Rng = {seed: 42}\n"
        "const x = $rng.f(0)\n", NULL) != 0);

    assert(ecs_script_run(world, NULL,
        "using flecs.script.math\n"
        "const rng: Rng = {seed: 42}\n"
        "const x = $rng.u(0)\n", NULL) != 0);
    ecs_log_set_level(-1);

    ecs_entity_t value_id = rng_test_value_type(world);
    test_rng_first_draw_uniform(world, value_id, 6);
    test_rng_first_draw_uniform(world, value_id, 16);
    test_rng_adjacent_seeds_differ(world, value_id);
    test_rng_same_seed_same_sequence(world, value_id);

    return ecs_fini(world);
}
