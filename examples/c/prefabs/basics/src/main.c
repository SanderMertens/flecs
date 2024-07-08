#include <basics.h>
#include <stdio.h>

// Prefabs are entities that can be used as templates for other entities. They
// are created with a builtin Prefab tag, which by default excludes them from
// queries and systems.
//
// Prefab instances are entities that have an IsA relationship to the prefab.
// The IsA relationship causes instances to inherit the components from the 
// prefab. By default all instances for a prefab share its components. 
//
// Inherited components save memory as they only need to be stored once for all
// prefab instances. They also speed up the creation of prefabs, as inherited
// components don't need to be copied to the instances.
//
// To get a private copy of a component, an instance can add it which is called 
// an override. Overrides can be manual (by using add) or automatic (see the
// override example).
//
// If a prefab has children, adding the IsA relationship instantiates the prefab
// children for the instance (see hierarchy example).

typedef struct { double value; } Defense;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Defense);

    // Make Defense component inheritable. By default components are copied from
    // the instance to the prefab. An inherited component is only stored on the
    // prefab, and is shared across all instances.
    ecs_add_pair(ecs, ecs_id(Defense), EcsOnInstantiate, EcsInherit);

    // Create a SpaceShip prefab with a Defense component.
    ecs_entity_t SpaceShip = ecs_entity(ecs, { .name = "SpaceShip", .add = ecs_ids( EcsPrefab ) });
    ecs_set(ecs, SpaceShip, Defense, {50});

    // Create a prefab instance
    ecs_entity_t inst = ecs_entity(ecs, { .name = "my_spaceship" });
    ecs_add_pair(ecs, inst, EcsIsA, SpaceShip);

    // Because of the IsA relationship, the instance now shares the Defense
    // component with the prefab, and can be retrieved as a regular component:
    const Defense *d_inst = ecs_get(ecs, inst, Defense);
    printf("defense: %f\n", d_inst->value);

    // Because the component is shared, changing the value on the prefab will
    // also change the value for the instance:
    ecs_set(ecs, SpaceShip, Defense, { 100 });
    printf("defense after set: %f\n", d_inst->value); // now prints 100

    // Prefab components can be iterated like regular components:
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Defense) }
        }
    });

    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        Defense *d = ecs_field(&it, Defense, 0);
        for (int i = 0; i < it.count; i ++) {
            printf("%s: defense: %f\n", 
                ecs_get_name(ecs, it.entities[i]), d[i].value);
        }
    }

    // Output:
    //  defense: 50.000000
    //  defense after set: 100.000000
    //  my_spaceship: defense: 100.000000

    return ecs_fini(ecs);
}


