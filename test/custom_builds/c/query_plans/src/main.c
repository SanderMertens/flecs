#include <query_plans.h>

#ifdef FLECS_QUERY_PLANS
#error "FLECS_QUERY_PLANS should not be enabled"
#endif

typedef struct Position {
    int x;
} Position;

static
int count_query(
    ecs_world_t *world,
    ecs_query_t *q)
{
    int result = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        result += it.count;
    }
    return result;
}

static
bool query_has(
    ecs_query_t *q,
    ecs_entity_t e)
{
    ecs_table_t *table = ecs_get_table(q->world, e);
    ecs_table_range_t range = { .table = table };
    ecs_iter_t it;

    if (!ecs_query_has(q, e, &it)) {
        return false;
    }
    ecs_iter_fini(&it);

    if (!ecs_query_has_table(q, table, &it)) {
        return false;
    }
    ecs_iter_fini(&it);

    if (!ecs_query_has_range(q, &range, &it)) {
        return false;
    }
    ecs_iter_fini(&it);

    return true;
}

static
bool query_set_this(
    ecs_world_t *world,
    ecs_query_t *q,
    ecs_entity_t e)
{
    ecs_table_t *table = ecs_get_table(world, e);
    ecs_table_range_t range = { .table = table };

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    if (!ecs_query_next(&it) || it.count != 1 || it.entities[0] != e ||
        ecs_query_next(&it))
    {
        return false;
    }

    it = ecs_query_iter(world, q);
    ecs_iter_set_var_as_table(&it, 0, table);
    if (!ecs_query_next(&it) || ecs_query_next(&it)) {
        return false;
    }

    it = ecs_query_iter(world, q);
    ecs_iter_set_var_as_range(&it, 0, &range);
    if (!ecs_query_next(&it) || ecs_query_next(&it)) {
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10});

    ecs_query_t *uncached = ecs_query(world, {
        .cache_kind = EcsQueryCacheNone,
        .terms = {{ ecs_id(Position) }}
    });
    if (!uncached || count_query(world, uncached) != 1 ||
        !query_has(uncached, e) || !query_set_this(world, uncached, e))
    {
        return -1;
    }

    ecs_query_t *cached = ecs_query(world, {
        .cache_kind = EcsQueryCacheAll,
        .terms = {{ ecs_id(Position) }}
    });
    if (!cached || count_query(world, cached) != 1 ||
        !query_has(cached, e) || !query_set_this(world, cached, e))
    {
        return -1;
    }

    ecs_log_set_level(-5);
    ecs_query_t *needs_plan = ecs_query(world, {
        .cache_kind = EcsQueryCacheNone,
        .terms = {
            { ecs_id(Position) },
            { Tag, .oper = EcsNot }
        }
    });
    if (needs_plan || ecs_log_last_error() != ECS_UNSUPPORTED) {
        return -1;
    }

    ecs_query_fini(cached);
    ecs_query_fini(uncached);

    return ecs_fini(world);
}
