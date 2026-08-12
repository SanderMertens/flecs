#include <metrics.h>

struct Position {
    float x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    ecs.import<flecs::metrics>();

    ecs_struct_desc_t sd = {};
    sd.entity = ecs.component<Position>("Position");
    sd.members[0].name = "x";
    sd.members[0].type = flecs::F32;
    sd.members[1].name = "y";
    sd.members[1].type = flecs::F32;
    sd.create_member_entities = true;
    ecs_entity_t pos = ecs_struct_init(ecs, &sd);
    assert(pos != 0);

    ecs_metric_desc_t desc = {};
    desc.entity = ecs.entity("metrics::position_y");
    desc.member = ecs.lookup("Position::y");
    desc.kind = EcsGauge;
    ecs_entity_t m = ecs_metric_init(ecs, &desc);
    assert(m != 0);

    ecs.entity().set<Position>({10, 20});

    ecs.progress();

    int32_t count = 0;
    ecs_iter_t it = ecs_children(ecs, m);
    while (ecs_children_next(&it)) {
        count += it.count;
    }
    assert(count == 1);

    return 0;
}
