#include <pipeline_w_system.h>

typedef struct {
    int32_t x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_dependson(EcsOnUpdate)) }),
        .query.terms = {{ .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }},
        .callback = Move
    });
    assert(s != 0);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    ecs_progress(world, 0);

    const Position *p = ecs_get(world, e, Position);
    assert(p->x == 11);
    assert(p->y == 22);

    return ecs_fini(world);
}
