#include <cached_queries.h>

typedef struct {
    int32_t x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .cache_kind = EcsQueryCacheAll,
        .terms = {{ .id = ecs_id(Position) }}
    });
    assert(q != NULL);
    assert(q->cache_kind == EcsQueryCacheAll);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        for (int i = 0; i < it.count; i ++) {
            assert(it.entities[i] == e);
            assert(p[i].x == 10);
            assert(p[i].y == 20);
            count ++;
        }
    }
    assert(count == 1);

    ecs_remove(world, e, Position);

    count = 0;
    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        count += it.count;
    }
    assert(count == 0);

    ecs_query_fini(q);

    return ecs_fini(world);
}
