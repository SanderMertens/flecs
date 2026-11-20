#include <script.h>
#include <stdio.h>

void install_test_abort(void) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-5);
}

bool script_map_has(
    ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    const char *expr_elem)
{
    char *expr = ecs_ptr_to_expr(world, type, ptr);
    if (!expr) {
        return false;
    }

    bool result = false;
    ecs_size_t len = ecs_os_strlen(expr_elem);
    const char *cur = expr;
    while ((cur = strstr(cur, expr_elem))) {
        char before = cur == expr ? '\0' : cur[-1];
        char after = cur[len];
        if ((before == '[' || before == ' ') && (after == ',' || after == ']')) {
            result = true;
            break;
        }
        cur ++;
    }

    ecs_os_free(expr);
    return result;
}
