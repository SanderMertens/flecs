#include <script_math.h>

int main(int, char *[]) {
    flecs::world ecs;

    ECS_IMPORT(ecs, FlecsScriptMath);

    int32_t v = 0;
    ecs_value_t value = {};
    value.type = ecs_id(ecs_i32_t);
    value.ptr = &v;

    assert(ecs_expr_run(ecs, "flecs.script.math.sqr(10)",
        &value, NULL) != NULL);
    assert(v == 100);

    return 0;
}
