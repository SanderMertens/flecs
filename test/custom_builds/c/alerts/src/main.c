#include <alerts.h>

typedef struct {
    int32_t x, y;
} Position, Velocity;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Position);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_entity(world, { .name = "position_without_velocity" }),
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .oper = EcsNot }
        }
    });
    assert(alert != 0);

    ecs_progress(world, 1.0);

    assert(!ecs_has(world, e1, EcsAlertsActive));
    assert(ecs_has(world, e2, EcsAlertsActive));
    assert(ecs_count(world, EcsAlertInstance) == 1);
    assert(ecs_get_alert_count(world, e2, alert) == 1);

    return ecs_fini(world);
}
