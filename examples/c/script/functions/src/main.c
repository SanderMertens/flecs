#include <functions.h>
#include <stdio.h>

/* This example shows how to use user-defined functions in scripts. */

const char *code = 
    "e {\n"
    "  Position: { sum(10, 20), (30).add(40) }\n"
    "}\n"
    ;

typedef struct {
    int32_t x, y;
} Position;

void sum(
    const ecs_function_ctx_t *ctx,
    int argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;

    *(int64_t*)result->ptr = 
        *(int64_t*)argv[0].ptr +
        *(int64_t*)argv[1].ptr;
}

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

    // Define a sum function
    ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_i64_t),
        .params = {
            { .name = "a", .type = ecs_id(ecs_i64_t) },
            { .name = "b", .type = ecs_id(ecs_i64_t) }
        },
        .callback = sum
    });

    // Define an add method. A method is like a function, except that it can be
    // called on type instances, and receives the instance as first parameter.
    ecs_method(world, {
        .name = "add",
        .parent = ecs_id(ecs_i64_t), // Add method to i64
        .return_type = ecs_id(ecs_i64_t),
        .params = {
            { .name = "a", .type = ecs_id(ecs_i64_t) }
        },
        .callback = sum
    });

    if (ecs_script_run(world, "My script", code, NULL)) {
        printf("script failed to run\n");
        return -1;
    }

    // Get entity and Position component
    ecs_entity_t e = ecs_lookup(world, "e");
    const Position *p = ecs_get(world, e, Position);

    printf("{x: %d, y: %d}\n", p->x, p->y);

    return ecs_fini(world);

    // Output
    //  {30, 70}
}
