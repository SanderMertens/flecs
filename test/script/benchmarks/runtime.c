#include <flecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int main(int argc, char **argv) {
    const char *mode = argc > 1 ? argv[1] : "resolve";
    int n = argc > 2 ? atoi(argv[2]) : 1000;
    bool ir = argc > 3 ? atoi(argv[3]) : true;
    if (n <= 0 || (strcmp(mode, "resolve") && strcmp(mode, "owners") &&
        strcmp(mode, "functions") && strcmp(mode, "tasks") &&
        strcmp(mode, "reactivity")))
    {
        return 1;
    }
    ecs_world_t *world = ecs_init();
    ecs_script_eval_desc_t desc = {.ir = ir};
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    if (!strcmp(mode, "resolve")) {
        for (int i = 0; i < n; i ++) {
            ecs_strbuf_append(&buf, "e%d {}\n", i);
        }
    } else if (!strcmp(mode, "owners")) {
        ecs_strbuf_appendstr(&buf, "struct Value(v: i32)\n");
        for (int i = 0; i < n; i ++) {
            ecs_strbuf_append(&buf, "e%d { child { Value: {%d} } }\n", i, i);
        }
    } else if (!strcmp(mode, "reactivity")) {
        ecs_entity_t type = ecs_struct(world, {
            .entity = ecs_entity(world, {.name = "Value"}),
            .members = {{"v", ecs_id(ecs_i32_t)}}
        });
        ecs_entity_t source = ecs_entity(world, {.name = "source"});
        ecs_set_id(world, source, type, sizeof(int32_t), &(int32_t){0});
        for (int i = 0; i < n; i ++) {
            ecs_strbuf_append(&buf,
                "e%d { Value: {source[Value].v} }\n", i);
        }
    } else if (!strcmp(mode, "functions")) {
        for (int i = 0; i < n; i ++) {
            ecs_strbuf_append(&buf, "fn f%d(a: i32) -> i32 { a + 1 }\n", i);
        }
    } else {
        ecs_strbuf_appendstr(&buf, "const a = 1\n");
    }
    char *code = ecs_strbuf_get(&buf);
    double start = now();
    if (!strcmp(mode, "owners") || !strcmp(mode, "reactivity")) {
        if (!ecs_script(world, {.code = code, .ir = ir})) return 1;
        ecs_entity_t type = ecs_lookup(world, "Value");
        if (!type) return 8;
        if (!strcmp(mode, "reactivity")) {
            ecs_entity_t source = ecs_lookup(world, "source");
            if (!source) return 9;
            start = now();
            for (int32_t i = 1; i <= 100; i ++) {
                ecs_set_id(world, source, type, sizeof(i), &i);
            }
        }
        char name[128];
        snprintf(name, sizeof(name), !strcmp(mode, "owners")
            ? "e%d.child" : "e%d", n - 1);
        ecs_entity_t entity = ecs_lookup(world, name);
        if (!entity) return 10;
        const int32_t *value = ecs_get_id(world, entity, type);
        if (!value || *value != (!strcmp(mode, "owners") ? n - 1 : 100)) {
            return 11;
        }
    } else {
        ecs_script_t *script = ecs_script_parse(world, "bench", code, &desc, NULL);
        if (!script) return 2;
        if (!strcmp(mode, "tasks")) {
            ecs_script_task_t **tasks = malloc(n * sizeof(*tasks));
            for (int i = 0; i < n; i ++) {
                tasks[i] = ecs_script_task_new(script, NULL);
                if (!tasks[i]) return 3;
            }
            for (int i = 0; i < n; i ++) {
                if (ecs_script_task_resume(tasks[i], NULL) != EcsScriptTaskDone) return 4;
                ecs_script_task_free(tasks[i]);
            }
            free(tasks);
        } else {
            if (ecs_script_eval(script, &desc, NULL)) return 5;
            if (!strcmp(mode, "functions")) {
                char call[128];
                snprintf(call, sizeof(call), "f%d(10)", n - 1);
                ecs_script_t *expr = ecs_expr_parse(world, call, NULL);
                if (!expr) return 6;
                start = now();
                for (int i = 0; i < 10000; i ++) {
                    int32_t result = 0;
                    ecs_value_t value = {ecs_id(ecs_i32_t), &result};
                    if (ecs_expr_eval(expr, &value, NULL) || result != 11) return 7;
                }
                ecs_script_free(expr);
            }
        }
        ecs_script_free(script);
    }
    printf("%s %d ir=%d %.6f\n", mode, n, ir, now() - start);
    ecs_os_free(code);
    ecs_fini(world);
    return 0;
}
