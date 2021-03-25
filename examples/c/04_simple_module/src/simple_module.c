#include <simple_module.h>

/* Implement a simple move system */
void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;

        printf("%s moved to {.x = %f, .y = %f}\n",
            ecs_get_name(it->world, it->entities[i]),
            p[i].x, p[i].y);
    }
}

void SimpleModuleImport(
    ecs_world_t *world)
{
    /* Define module */
    ECS_MODULE(world, SimpleModule);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Define a system called Move that is executed every frame, and subscribes
     * for the 'Position' and 'Velocity' components */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    /* Export handles to module contents */
    ECS_SET_COMPONENT(Position);
    ECS_SET_COMPONENT(Velocity);
    ECS_SET_ENTITY(Move);
}
