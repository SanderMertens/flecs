#include <perf_trace.h>

static int push_invoked = 0;
static int pop_invoked = 0;

static void trace_push(const char *file, size_t line, const char *name) {
    (void)file;
    (void)line;
    (void)name;
    push_invoked ++;
}

static void trace_pop(const char *file, size_t line, const char *name) {
    (void)file;
    (void)line;
    (void)name;
    pop_invoked ++;
}

int main(int, char *[]) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_get_api();
    os_api.perf_trace_push_ = trace_push;
    os_api.perf_trace_pop_ = trace_pop;
    ecs_os_set_api(&os_api);

    {
        flecs::world ecs;
        ecs.progress();
    }

    assert(push_invoked != 0);
    assert(pop_invoked != 0);
    assert(push_invoked == pop_invoked);

    return 0;
}
