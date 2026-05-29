#include <component_inheritance.h>
#include <stdio.h>

// This example shows how queries can match components that derive from a base
// component through the IsA relationship. A query for the base component
// matches all entities that have a derived component, and can read the members
// that are inherited from the base. See the tag_inheritance example for
// inheritance trees built from tags instead of components.

// Buf is the base component. HealthBuf and ManaBuf derive from it, so they
// start with the same members as Buf (the base layout is a prefix of the
// derived layout).
typedef struct {
    float value;
} Buf;

typedef struct {
    float value;
} HealthBuf;

typedef struct {
    float value;
} ManaBuf;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Buf);
    ECS_COMPONENT(ecs, HealthBuf);
    ECS_COMPONENT(ecs, ManaBuf);

    // Make the ECS aware of the inheritance relationships.
    ecs_add_pair(ecs, ecs_id(HealthBuf), EcsIsA, ecs_id(Buf));
    ecs_add_pair(ecs, ecs_id(ManaBuf), EcsIsA, ecs_id(Buf));

    // Create a few entities with derived buf components
    ecs_entity_t warrior = ecs_entity(ecs, { .name = "warrior" });
    ecs_set(ecs, warrior, HealthBuf, { .value = 10 });

    ecs_entity_t wizard = ecs_entity(ecs, { .name = "wizard" });
    ecs_set(ecs, wizard, ManaBuf, { .value = 25 });

    ecs_entity_t paladin = ecs_entity(ecs, { .name = "paladin" });
    ecs_set(ecs, paladin, HealthBuf, { .value = 5 });

    // Create a query for the base component. This matches all entities with a
    // component that derives from Buf.
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {{ .id = ecs_id(Buf) }}
    });

    // Iterate the query. Because the matched component can be a derived type
    // that is larger than Buf, use ecs_base_field to get the field pointer and
    // ecs_field_stride to advance from one element to the next.
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        void *bufs = ecs_base_field(&it, Buf, 0);
        size_t stride = ecs_field_stride(&it, 0);
        for (int i = 0; i < it.count; i ++) {
            Buf *buf = ECS_OFFSET(bufs, stride * i);
            printf("%s has buf value %.0f\n",
                ecs_get_name(ecs, it.entities[i]), buf->value);
        }
    }

    ecs_query_fini(q);

    // Output
    //  warrior has buf value 10
    //  paladin has buf value 5
    //  wizard has buf value 25

    return ecs_fini(ecs);
}
