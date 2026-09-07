#include "addons/script/script.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int events;

static void changed(ecs_iter_t *it) {
    (void)it;
    events ++;
}

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int main(int argc, char **argv) {
    int n = argc > 1 ? atoi(argv[1]) : 1024;
    int repeat = argc > 2 ? atoi(argv[2]) : 1000;
    if (n < 2 || repeat < 1) return 1;
    ecs_world_t *world = ecs_init();
    ecs_entity_t component = ecs_new(world);
    ecs_entity_t script = ecs_new(world);
    ecs_vec_t refs = {0}, observers = {0};
    for (int i = 0; i < n; i ++) {
        ecs_script_ref_t *r = ecs_vec_append_t(NULL, &refs, ecs_script_ref_t);
        *r = (ecs_script_ref_t){
            .entity = ecs_new(world), .component = component,
            .is_has = true, .input = 1
        };
    }
    flecs_script_update_ref_observers(world, script, 0, &refs, &observers, changed);
    ecs_script_ref_t *expected = ecs_vec_first(&refs);
    ecs_script_ref_t *actual = ecs_vec_first(&observers);
    ecs_entity_t *ids = malloc(n * sizeof(*ids));
    for (int i = 0; i < n; i ++) ids[i] = actual[i].observer;
    int64_t allocs = ecs_os_api_malloc_count + ecs_os_api_realloc_count + ecs_os_api_calloc_count;
    double start = now();
    for (int i = 0; i < repeat; i ++) {
        flecs_script_update_ref_observers(world, script, 0, &refs, &observers, changed);
    }
    double elapsed = now() - start;
    allocs = ecs_os_api_malloc_count + ecs_os_api_realloc_count + ecs_os_api_calloc_count - allocs;
    actual = ecs_vec_first(&observers);
    for (int i = 0; i < n; i ++) if (actual[i].observer != ids[i]) return 2;
    for (int i = 0; i < n / 2; i ++) {
        ecs_script_ref_t tmp = expected[i];
        expected[i] = expected[n - i - 1];
        expected[n - i - 1] = tmp;
    }
    flecs_script_update_ref_observers(world, script, 0, &refs, &observers, changed);
    actual = ecs_vec_first(&observers);
    for (int i = 0; i < n; i ++) {
        if (actual[i].observer != ids[n - i - 1]) return 3;
        ecs_add_id(world, expected[i].entity, component);
    }
    if (events != n) return 4;
    ecs_vec_set_count_t(NULL, &refs, ecs_script_ref_t, n / 2);
    flecs_script_update_ref_observers(world, script, 0, &refs, &observers, changed);
    for (int i = 0; i < n - n / 2; i ++) if (ecs_is_alive(world, ids[i])) return 5;
    flecs_script_ref_observers_clear(world, &observers);
    ecs_vec_fini_t(NULL, &refs, ecs_script_ref_t);
    free(ids);
    printf("observers n=%d repeats=%d %.6f allocations=%lld\n", n, repeat, elapsed, (long long)allocs);
    return ecs_fini(world);
}
