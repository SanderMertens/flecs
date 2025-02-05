#include <query_dsl.h>
#include <stdio.h>

typedef struct {
    float x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position"
    });

    if (!q) {
        printf("failed to create query\n");
        return -1;
    }

    ecs_id_t id = ecs_id_from_str(world, "(Position, Foo)");
    if (!id || id != ecs_pair_t(Position, Foo)) {
        printf("failed to parse id\n");
        return -1;
    }

    return ecs_fini(world);
}
