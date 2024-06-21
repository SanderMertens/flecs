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
