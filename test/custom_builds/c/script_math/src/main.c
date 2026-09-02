#include <script_math.h>
#include <stdio.h>

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

    return ecs_fini(world);
}
