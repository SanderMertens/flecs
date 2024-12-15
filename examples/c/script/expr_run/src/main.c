#include <expr_run.h>
#include <stdio.h>

// This example shows how to run an expression once. To see how to parse an 
// expression and run it multiple times, see the expr_w_vars example.

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Declare value that will hold the expression result. Specify that we want
    // the returned value to be of type i32. If the expression type doesn't 
    // match the specified type, it will be casted.
    int32_t result = 0;
    ecs_value_t result_value = {
        .type = ecs_id(ecs_i32_t),
        .ptr = &result
    };

    // Run the expression. If the operation is successful it returns a pointer
    // to the character after the last parsed token.
    if (ecs_expr_run(world, "10 + 20", &result_value, NULL) == NULL) {
        printf("expression failed to run\n");
        return -1;
    }

    printf("result = %d\n", result);

    return ecs_fini(world);

    // Output
    //  result = 30
}
