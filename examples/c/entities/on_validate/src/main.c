#include <on_validate.h>
#include <stdio.h>

// The on_validate hook is invoked when a component is set, before the on_set
// hook and OnSet observers are invoked. When the hook returns false, the
// on_set hook and OnSet observers are not invoked for the entity. This can be
// used to guard application logic from invalid component values.

typedef struct {
    double x, y;
} Position;

// Returns false when the position is outside of the world bounds.
bool validate_position(ecs_world_t *world, ecs_entity_t e, void *ptr) {
    (void)world; (void)e;
    Position *p = ptr;
    return p->x >= 0 && p->x <= 100 && p->y >= 0 && p->y <= 100;
}

void on_set_position(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);

    for (int i = 0; i < it->count; i ++) {
        printf("%s set to {%.0f, %.0f}\n",
            ecs_get_name(it->world, it->entities[i]), p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_validate = validate_position,
        .on_set = on_set_position
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Entity" });

    // Position is inside world bounds, on_set is invoked
    ecs_set(world, e, Position, {50, 50});

    // Position is outside world bounds, on_set is not invoked
    ecs_set(world, e, Position, {150, 50});

    return ecs_fini(world);

    // Output:
    //   Entity set to {50, 50}
}
