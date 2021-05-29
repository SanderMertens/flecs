#include <observer.h>

typedef struct {
    float x, y;
} Position, Velocity;

void observer_callback(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        printf("observer triggered!\n");
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t Likes = ecs_new_id(world);
    ecs_entity_t Apples = ecs_new_id(world);

    /* An observer triggers when an event matching one of its terms matches, and
     * the entity matches with all other terms. */
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            {.id = ecs_id(Position)}, 
            {.id = ecs_id(Velocity)}, 
            {.id = ecs_pair(Likes, EcsWildcard)}
        },
        .events = {EcsOnAdd},
        .callback = observer_callback
    });

    ecs_entity_t e = ecs_new_id(world);

    ecs_add(world, e, Position); /* Observer does not trigger */

    ecs_add(world, e, Velocity); /* Observer does not trigger */

    /* Observer triggers, entity matches all of its terms */
    ecs_add_pair(world, e, Likes, Apples);

    return ecs_fini(world);
}
