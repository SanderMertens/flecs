#include <timer.h>

typedef struct {
    float x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

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
        .query.filter.terms = {{ .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }},
        .callback = Move,
        .entity.add = { ecs_dependson(EcsOnUpdate) },
        .interval = 2.9f
    });
    assert(s != 0);

    ecs_entity_t e = ecs_new_id(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    const Position *p = ecs_get(world, e, Position);

    ecs_progress(world, 1.0);
    ecs_progress(world, 1.0);
    
    assert(p->x == 10);
    assert(p->y == 20);

    ecs_progress(world, 1.0);

    assert(p->x == 11);
    assert(p->y == 22);

    return ecs_fini(world);
}
