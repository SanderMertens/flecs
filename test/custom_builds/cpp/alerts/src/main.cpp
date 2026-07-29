#include <alerts.h>

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    ECS_IMPORT(ecs, FlecsAlerts);

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    ecs_alert_desc_t desc = {};
    desc.entity = ecs.entity("position_without_velocity");
    desc.query.terms[0].id = ecs.id<Position>();
    desc.query.terms[1].id = ecs.id<Velocity>();
    desc.query.terms[1].oper = EcsNot;
    ecs_entity_t alert = ecs_alert_init(ecs, &desc);
    assert(alert != 0);

    ecs.progress(1.0);

    assert(!ecs_has_id(ecs, e1, ecs_id(EcsAlertsActive)));
    assert(ecs_has_id(ecs, e2, ecs_id(EcsAlertsActive)));
    assert(ecs_get_alert_count(ecs, e2, alert) == 1);

    return 0;
}
