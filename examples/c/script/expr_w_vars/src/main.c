#include <expr_w_vars.h>
#include <stdio.h>

// This example shows how to run an expression that uses variables. Variables
// make it possible to change the inputs to an expression after the expression
// has been parsed. This example also shows how to parse an expression and 
// evaluate it multiple times.

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Declare variables that we'll use in the expression
    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *x = ecs_script_vars_define(vars, "x", ecs_i32_t);
    ecs_script_var_t *y = ecs_script_vars_define(vars, "y", ecs_i32_t);

    // Assign values to variables
    *(int32_t*)x->value.ptr = 10;
    *(int32_t*)y->value.ptr = 20;

    // Declare value that will hold the expression result. Specify that we want
    // the returned value to be of type i32. If the expression type doesn't 
    // match the specified type, it will be casted.
    int32_t result = 0;
    ecs_value_t result_value = {
        .type = ecs_id(ecs_i32_t),
        .ptr = &result
    };

    // Parse expression
    ecs_expr_eval_desc_t desc = { .vars = vars };
    ecs_script_t *s = ecs_expr_parse(world, "$x + $y", &desc);
    if (!s) {
        printf("failed to parse expression\n");
        return -1;
    }

    // Evaluate expression with variables
    if (ecs_expr_eval(s, &result_value, &desc)) {
        printf("failed to evaluate expression\n");
        return -1;
    }

    printf("result = %d\n", result);

    // Change variable and reevaluate expression
    *(int32_t*)y->value.ptr = 30;
    if (ecs_expr_eval(s, &result_value, &desc)) {
        printf("failed to evaluate expression\n");
        return -1;
    }

    printf("result = %d\n", result);

    // Free resources
    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    return ecs_fini(world);

    // Output
    //  result = 30
    //  result = 40
}
