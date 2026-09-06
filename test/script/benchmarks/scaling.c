#include <flecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef union allocation_t {
    size_t size;
    long double alignment;
    char padding[16];
} allocation_t;

static ecs_os_api_t original;
static int64_t live_bytes;
static int64_t allocations;
static ecs_script_future_t **futures;
static int future_count;

static void* tracked_malloc(ecs_size_t size) {
    allocation_t *p = original.malloc_(size + sizeof(allocation_t));
    p->size = size;
    live_bytes += size;
    allocations ++;
    return p + 1;
}

static void* tracked_calloc(ecs_size_t size) {
    void *p = tracked_malloc(size);
    memset(p, 0, size);
    return p;
}

static void tracked_free(void *ptr) {
    if (ptr) {
        allocation_t *p = (allocation_t*)ptr - 1;
        live_bytes -= p->size;
        original.free_(p);
    }
}

static void* tracked_realloc(void *ptr, ecs_size_t size) {
    if (!ptr) return tracked_malloc(size);
    allocation_t *p = (allocation_t*)ptr - 1;
    live_bytes += size - (int64_t)p->size;
    p = original.realloc_(p, size + sizeof(allocation_t));
    p->size = size;
    allocations ++;
    return p + 1;
}

static char* tracked_strdup(const char *str) {
    if (!str) return NULL;
    size_t size = strlen(str) + 1;
    char *result = tracked_malloc((ecs_size_t)size);
    memcpy(result, str, size);
    return result;
}

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

static void pending(const ecs_function_ctx_t *ctx, int32_t argc,
    const ecs_value_t *argv, ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;
    (void)argv;
    futures[future_count ++] = future;
}

int main(int argc, char **argv) {
    if (argc < 4) return 1;
    const char *mode = argv[1];
    int n = atoi(argv[2]);
    bool ir = atoi(argv[3]) != 0;
    int depth = argc > 4 ? atoi(argv[4]) : 1;
    if (n < 1 || depth < 1 || depth > 32) return 1;

    ecs_os_set_api_defaults();
    original = ecs_os_api;
    ecs_os_api_t api = original;
    api.malloc_ = tracked_malloc;
    api.calloc_ = tracked_calloc;
    api.realloc_ = tracked_realloc;
    api.free_ = tracked_free;
    api.strdup_ = tracked_strdup;
    ecs_os_set_api(&api);

    ecs_world_t *world = ecs_init();
    ecs_script_eval_desc_t desc = {.ir = ir};
    double elapsed = 0;
    int64_t bytes = 0, allocs = 0;
    if (!strcmp(mode, "continuations")) {
        futures = malloc((n + 1) * sizeof(*futures));
        ecs_async_function(world, {
            .name = "pending", .return_type = ecs_id(ecs_i32_t),
            .callback = pending
        });
        ecs_strbuf_t buf = ECS_STRBUF_INIT;
        for (int i = 0; i < depth; i ++) {
            ecs_strbuf_appendstr(&buf, "if true {\n");
            ecs_strbuf_append(&buf, "const x%d = %d\n", i, i);
        }
        ecs_strbuf_appendstr(&buf, "const result = await pending()\n");
        for (int i = 0; i < depth; i ++) ecs_strbuf_appendstr(&buf, "}\n");
        char *code = ecs_strbuf_get(&buf);
        ecs_script_t *script = ecs_script_parse(world, NULL, code, &desc, NULL);
        if (!script) return 2;
        ecs_os_free(code);
        ecs_script_task_t *warm = ecs_script_task_new(script, NULL);
        if (!warm || ecs_script_task_resume(warm, NULL) != EcsScriptTaskPending) return 3;
        ecs_script_task_free(warm);
        ecs_script_future_release(futures[0]);
        future_count = 0;
        ecs_script_task_t **tasks = malloc(n * sizeof(*tasks));
        int64_t before_bytes = live_bytes, before_allocs = allocations;
        double start = now();
        for (int i = 0; i < n; i ++) {
            tasks[i] = ecs_script_task_new(script, NULL);
            if (!tasks[i] || ecs_script_task_resume(tasks[i], NULL) != EcsScriptTaskPending) return 4;
        }
        elapsed = now() - start;
        bytes = live_bytes - before_bytes;
        allocs = allocations - before_allocs;
        if (future_count != n) return 5;
        for (int i = 0; i < n; i ++) {
            ecs_value_t result = ecs_value(ecs_i32_t, {42});
            if (ecs_script_future_resolve(futures[i], &result)) return 6;
            ecs_script_future_release(futures[i]);
            if (ecs_script_task_resume(tasks[i], NULL) != EcsScriptTaskDone) return 7;
            ecs_script_task_free(tasks[i]);
        }
        free(tasks);
        free(futures);
        ecs_script_free(script);
    } else if (!strcmp(mode, "functions")) {
        if (ecs_script_run_w_desc(world, NULL,
            "fn add(a: i32, b: i32) -> i32 { a + b }", &desc, NULL)) return 8;
        ecs_script_t *expr = ecs_expr_parse(world, "add(10, 20)", NULL);
        if (!expr) return 9;
        int32_t value = 0;
        ecs_value_t result = {ecs_id(ecs_i32_t), &value};
        ecs_expr_eval_desc_t expr_desc = {.runtime = ecs_script_runtime_new()};
        for (int i = 0; i < 100; i ++) {
            if (ecs_expr_eval(expr, &result, &expr_desc) || value != 30) return 10;
        }
        int64_t before_allocs = allocations, before_bytes = live_bytes;
        double start = now();
        for (int i = 0; i < n; i ++) {
            if (ecs_expr_eval(expr, &result, &expr_desc) || value != 30) return 11;
        }
        elapsed = now() - start;
        allocs = allocations - before_allocs;
        bytes = live_bytes - before_bytes;
        ecs_script_free(expr);
        ecs_script_runtime_free(expr_desc.runtime);
    } else if (!strcmp(mode, "sparse") || !strcmp(mode, "dense")) {
        ecs_entity_t type = ecs_struct(world, {
            .entity = ecs_entity(world, {.name = "Value"}),
            .members = {{"v", ecs_id(ecs_i32_t)}}
        });
        ecs_entity_t hot = ecs_entity(world, {.name = "hot"});
        ecs_entity_t cold = ecs_entity(world, {.name = "cold"});
        ecs_set_id(world, hot, type, sizeof(int32_t), &(int32_t){0});
        ecs_set_id(world, cold, type, sizeof(int32_t), &(int32_t){0});
        ecs_strbuf_t buf = ECS_STRBUF_INIT;
        for (int i = 0; i < n; i ++) {
            const char *source = !strcmp(mode, "dense") || i == n / 2 ? "hot" : "cold";
            ecs_strbuf_append(&buf, "e%d { Value: {%s[Value].v} }\n", i, source);
        }
        char *code = ecs_strbuf_get(&buf);
        if (!ecs_script(world, {.code = code, .ir = ir})) return 12;
        ecs_os_free(code);
        int64_t before_allocs = allocations, before_bytes = live_bytes;
        double start = now();
        for (int32_t i = 1; i <= 1000; i ++) {
            ecs_set_id(world, hot, type, sizeof(i), &i);
        }
        elapsed = now() - start;
        allocs = allocations - before_allocs;
        bytes = live_bytes - before_bytes;
        for (int i = 0; i < n; i ++) {
            char name[32];
            snprintf(name, sizeof(name), "e%d", i);
            ecs_entity_t entity = ecs_lookup(world, name);
            if (!entity) return 13;
            const int32_t *value = ecs_get_id(world, entity, type);
            int32_t expected = !strcmp(mode, "dense") || i == n / 2 ? 1000 : 0;
            if (!value || *value != expected) return 14;
        }
    } else {
        return 1;
    }
    printf("%s n=%d ir=%d depth=%d seconds=%.9f bytes=%lld allocations=%lld\n",
        mode, n, ir, depth, elapsed, (long long)bytes, (long long)allocs);
    ecs_fini(world);
    if (live_bytes) {
        fprintf(stderr, "unreleased bytes: %lld\n", (long long)live_bytes);
        return 15;
    }
    return 0;
}
