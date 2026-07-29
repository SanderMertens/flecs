#include <metrics.h>

typedef struct {
    float x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsMetrics);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) },
        },
        .create_member_entities = true
    });

    ecs_entity_t m = ecs_metric(world, {
        .entity = ecs_entity(world, { .name = "metrics.position_y" }),
        .member = ecs_lookup(world, "Position.y"),
        .kind = EcsGauge
    });
    assert(m != 0);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_progress(world, 0);

    int32_t count = 0;
    ecs_iter_t it = ecs_children(world, m);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            const EcsMetricSource *src = ecs_get(
                world, it.entities[i], EcsMetricSource);
            const EcsMetricValue *value = ecs_get(
                world, it.entities[i], EcsMetricValue);
            assert(src != NULL);
            assert(value != NULL);
            assert(src->entity == e);
            assert((int32_t)value->value == 20);
            count ++;
        }
    }
    assert(count == 1);

    return ecs_fini(world);
}
