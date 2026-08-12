#include <query_plans.h>

typedef struct {
    int32_t x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {30, 40});
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query(world, {
        .cache_kind = EcsQueryCacheNone,
        .terms = {
            { .id = ecs_id(Position) },
            { .id = Tag, .oper = EcsNot }
        }
    });
    assert(q != NULL);

    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            assert(it.entities[i] == e1);
            count ++;
        }
    }
    assert(count == 1);
    ecs_query_fini(q);

    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtB);

    ecs_query_t *qw = ecs_query(world, {
        .cache_kind = EcsQueryCacheNone,
        .terms = {{ .id = ecs_pair(Rel, EcsWildcard) }}
    });
    assert(qw != NULL);

    count = 0;
    it = ecs_query_iter(world, qw);
    while (ecs_query_next(&it)) {
        count += it.count;
    }
    assert(count == 2);
    ecs_query_fini(qw);

    return ecs_fini(world);
}
