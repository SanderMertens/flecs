#include <no_addons.h>

#ifdef FLECS_CACHED_QUERIES
#error "FLECS_CACHED_QUERIES should not be enabled"
#endif

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_entity_t Tag = ecs_new(world);
    ecs_query_t *q = ecs_query(world, {
        .cache_kind = EcsQueryCacheAuto,
        .terms = {{ Tag }}
    });

    if (!q || q->cache_kind != EcsQueryCacheNone) {
        return -1;
    }

    ecs_query_fini(q);

    return ecs_fini(world);
}
