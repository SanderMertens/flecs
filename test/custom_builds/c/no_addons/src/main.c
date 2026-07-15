#include <no_addons.h>

#ifdef FLECS_CACHED_QUERIES
#error "FLECS_CACHED_QUERIES should not be enabled"
#endif

typedef struct Position {
    int x;
    int y;
} Position;

typedef struct Velocity {
    int x;
    int y;
} Velocity;

typedef struct Mass {
    int value;
} Mass;

static bool query_count_is(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t expected)
{
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(world, query);
    while (ecs_query_next(&it)) {
        count += it.count;
    }
    return count == expected;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q_position = ecs_query(world, {
        .cache_kind = EcsQueryCacheAuto,
        .terms = {{ .id = ecs_id(Position) }}
    });
    ecs_query_t *q_position_velocity = ecs_query(world, {
        .cache_kind = EcsQueryCacheAuto,
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        }
    });
    ecs_query_t *q_position_velocity_mass = ecs_query(world, {
        .cache_kind = EcsQueryCacheAuto,
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) },
            { .id = ecs_id(Mass) }
        }
    });

    if (!q_position || !q_position_velocity ||
        !q_position_velocity_mass ||
        q_position->cache_kind != EcsQueryCacheNone ||
        q_position_velocity->cache_kind != EcsQueryCacheNone ||
        q_position_velocity_mass->cache_kind != EcsQueryCacheNone)
    {
        return -1;
    }

    ecs_entity_t e_position = ecs_new(world);
    ecs_set(world, e_position, Position, {10, 20});

    ecs_entity_t e_position_velocity = ecs_new(world);
    ecs_set(world, e_position_velocity, Position, {30, 40});
    ecs_set(world, e_position_velocity, Velocity, {1, 2});

    ecs_entity_t e_position_velocity_mass = ecs_new(world);
    ecs_set(world, e_position_velocity_mass, Position, {50, 60});
    ecs_set(world, e_position_velocity_mass, Velocity, {3, 4});
    ecs_set(world, e_position_velocity_mass, Mass, {100});

    if (!ecs_is_alive(world, e_position) ||
        !ecs_is_alive(world, e_position_velocity) ||
        !ecs_is_alive(world, e_position_velocity_mass) ||
        !query_count_is(world, q_position, 3) ||
        !query_count_is(world, q_position_velocity, 2) ||
        !query_count_is(world, q_position_velocity_mass, 1))
    {
        return -1;
    }

    ecs_add(world, e_position, Velocity);
    if (!ecs_has(world, e_position, Velocity) ||
        !query_count_is(world, q_position_velocity, 3))
    {
        return -1;
    }

    ecs_remove(world, e_position, Velocity);
    if (ecs_has(world, e_position, Velocity) ||
        !query_count_is(world, q_position_velocity, 2))
    {
        return -1;
    }

    ecs_add(world, e_position_velocity, Mass);
    if (!ecs_has(world, e_position_velocity, Mass) ||
        !query_count_is(world, q_position_velocity_mass, 2))
    {
        return -1;
    }

    ecs_remove(world, e_position_velocity, Mass);
    if (ecs_has(world, e_position_velocity, Mass) ||
        !query_count_is(world, q_position_velocity_mass, 1))
    {
        return -1;
    }

    ecs_delete(world, e_position_velocity_mass);
    if (ecs_is_alive(world, e_position_velocity_mass) ||
        !query_count_is(world, q_position, 2) ||
        !query_count_is(world, q_position_velocity, 1) ||
        !query_count_is(world, q_position_velocity_mass, 0))
    {
        return -1;
    }

    ecs_query_fini(q_position_velocity_mass);
    ecs_query_fini(q_position_velocity);
    ecs_query_fini(q_position);

    return ecs_fini(world);
}
