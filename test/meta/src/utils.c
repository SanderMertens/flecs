#include <meta.h>

void install_test_abort(void) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_get_api();
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-5);
}

void _test_json(const char* str1, const char *str2, const char* vstr1, const char *vstr2, const char *file, int line){
    if (ecs_os_strcmp(str1, str2)) {
        char *esc = flecs_astresc('"', str1);
        printf("esc: %s\n", esc);
        ecs_os_free(esc);
    }
    _test_str(str1, str2, vstr1, vstr2, file, line);
}

bool meta_map_has(
    ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    const char *json_elem)
{
    char *json = ecs_ptr_to_json(world, type, ptr);
    if (!json) {
        return false;
    }

    bool result = false;
    ecs_size_t len = ecs_os_strlen(json_elem);
    const char *cur = json;
    while ((cur = strstr(cur, json_elem))) {
        char before = cur == json ? '\0' : cur[-1];
        char after = cur[len];
        if ((before == '{' || before == ' ') && (after == ',' || after == '}')) {
            result = true;
            break;
        }
        cur ++;
    }

    ecs_os_free(json);
    return result;
}
