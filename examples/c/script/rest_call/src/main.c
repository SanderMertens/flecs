#include <rest_call.h>
#include <stdio.h>
#include <string.h>

void sum(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;

    int32_t *a = argv[0].ptr;
    int32_t *b = argv[1].ptr;

    *(int32_t*)result->ptr = *a + *b;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { .name = "a", .type = ecs_id(ecs_i32_t) },
            { .name = "b", .type = ecs_id(ecs_i32_t) }
        },
        .callback = sum
    });

    printf("Try: curl 'http://localhost:27750/call/sum?a=10&b=20'\n");

    return ecs_app_run(world, &(ecs_app_desc_t){
        .enable_rest = true
    });
}
