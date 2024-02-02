#include <variant.h>
#include <stdio.h>

/* Prefabs can inherit from each other, which creates prefab variants. With
 * variants applications can reuse a commmon set of components and specialize it
 * by adding or overriding components on the variant. */

typedef struct { double value; } Attack;
typedef struct { double value; } Defense;
typedef struct { double value; } FreightCapacity;
typedef struct { double value; } ImpulseSpeed;

typedef struct {
    double x; 
    double y; 
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Attack);
    ECS_COMPONENT(ecs, Defense);
    ECS_COMPONENT(ecs, FreightCapacity);
    ECS_COMPONENT(ecs, ImpulseSpeed);

    // Create a base prefab for SpaceShips.
    ecs_entity_t SpaceShip = ecs_new_prefab(ecs, "SpaceShip");
    ecs_set(ecs, SpaceShip, ImpulseSpeed, {50});
    ecs_set(ecs, SpaceShip, Defense, {25});

        // Create a Freighter variant which inherits from SpaceShip
        ecs_entity_t Freighter = ecs_new_prefab(ecs, "Freighter");
        ecs_add_pair(ecs, Freighter, EcsIsA, SpaceShip);
        ecs_set(ecs, Freighter, FreightCapacity, {100});
        ecs_set(ecs, Freighter, Defense, {50});

            // Create a MammothFreighter variant which inherits from Freighter
            ecs_entity_t MammothFreighter = ecs_new_prefab(ecs, "MammothFreighter");
            ecs_add_pair(ecs, MammothFreighter, EcsIsA, Freighter);
            ecs_set(ecs, MammothFreighter, FreightCapacity, {500});

        // Create a Frigate variant which inherits from SpaceShip
        ecs_entity_t Frigate = ecs_new_prefab(ecs, "Frigate");
        ecs_add_pair(ecs, Frigate, EcsIsA, SpaceShip);
        ecs_set(ecs, Frigate, Attack, {100});
        ecs_set(ecs, Frigate, Defense, {75});
        ecs_set(ecs, Frigate, ImpulseSpeed, {125});


    // Create an instance of the MammothFreighter. This entity will inherit the
    // ImpulseSpeed from SpaceShip, Defense from Freighter and FreightCapacity
    // from MammothFreighter.
    ecs_entity_t inst = ecs_new_entity(ecs, "my_freighter");
    ecs_add_pair(ecs, inst, EcsIsA, MammothFreighter);

    // Add a private Position component.
    ecs_set(ecs, inst, Position, {10, 20});

    // Instances can override inherited components to give them a private copy
    // of the component. This freighter got an armor upgrade:
    ecs_set(ecs, inst, Defense, {100});

    // Queries can match components from multiple levels of inheritance
    ecs_query_t *q = ecs_query(ecs, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(ImpulseSpeed) }, 
            { .id = ecs_id(Defense) }, 
            { .id = ecs_id(FreightCapacity) }
        }
    });

    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        Position  *p = ecs_field(&it, Position, 1);
        ImpulseSpeed *s = ecs_field(&it, ImpulseSpeed, 2);
        Defense *d = ecs_field(&it, Defense, 3);
        FreightCapacity *c = ecs_field(&it, FreightCapacity, 4);

        for (int i = 0; i < it.count; i ++) {
            printf("%s:\n", ecs_get_name(ecs, it.entities[i]));
            printf(" - position: %f, %f\n", p[i].x, p[i].y);
            printf(" - impulse speed: %f\n", s[i].value);
            printf(" - defense: %f\n", d[i].value);
            printf(" - capacity: %f\n", c[i].value);
        }
    }

    // Note that when matching tables with shared components, entities are
    // returned one by one. See the queries/instancing example for more details.

    // Output:
    //  my_freighter:
    //   - position: 10.000000, 20.000000
    //   - impulse speed: 50.000000
    //   - defense: 100.000000
    //   - capacity: 500.000000

    return ecs_fini(ecs);
}
