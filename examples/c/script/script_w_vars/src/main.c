#include <script_w_vars.h>
#include <stdio.h>

/* This example shows how to run a script that takes variables as input. The 
 * example also shows how to parse a script once and run it multiple times. */

const char *code = 
    "e {\n"
    "  Position: {$x * 10, $y * 10}\n"
    "}\n"
    ;

typedef struct {
    int32_t x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);

    // Add reflection data to component so we can assign it in script
    ecs_struct(world, {
        .entity = ecs_id(Position), // Make sure to use existing id
        .members = {
            { .name = "x", .type = ecs_id(ecs_i32_t) },
            { .name = "y", .type = ecs_id(ecs_i32_t) }
        }
    });

    // Declare variables that we'll use in the expression
    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *x = ecs_script_vars_define(vars, "x", ecs_i32_t);
    ecs_script_var_t *y = ecs_script_vars_define(vars, "y", ecs_i32_t);

    ecs_script_eval_desc_t desc = { .vars = vars };

    // Parse the script
    ecs_script_t *script = ecs_script_parse(world, "My script", code, &desc, NULL);
    if (!script) {
        printf("script failed to parse\n");
        return -1;
    }

    // Assign values to variables and evaluate
    *(int32_t*)x->value.ptr = 1;
    *(int32_t*)y->value.ptr = 2;
    if (ecs_script_eval(script, &desc, NULL) != 0) {
        printf("script failed to run\n");
        return -1;
    }

    // Get entity and Position component
    ecs_entity_t e = ecs_lookup(world, "e");
    const Position *p = ecs_get(world, e, Position);

    printf("{x: %d, y: %d}\n", p->x, p->y);

    // Change values for variables and reevaluate
    *(int32_t*)x->value.ptr = 2;
    *(int32_t*)y->value.ptr = 3;
    if (ecs_script_eval(script, &desc, NULL) != 0) {
        printf("script failed to run\n");
        return -1;
    }

    printf("{x: %d, y: %d}\n", p->x, p->y);

    ecs_script_vars_fini(vars);
    ecs_script_free(script);

    return ecs_fini(world);

    // Output
    //  {10, 20}
    //  {20, 30}
}
