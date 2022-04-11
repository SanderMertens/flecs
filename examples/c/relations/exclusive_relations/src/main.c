#include <exclusive_relations.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Platoon);

    // Register Platoon as exclusive relationship. This ensures that an entity
    // can only belong to a single Platoon.
    ecs_add_id(ecs, Platoon, EcsExclusive);

    // Create two platoons
    ecs_entity_t platoon_1 = ecs_new_id(ecs);
    ecs_entity_t platoon_2 = ecs_new_id(ecs);

    // Create a unit
    ecs_entity_t unit = ecs_new_id(ecs);

    // Add unit to platoon 1
    ecs_add_pair(ecs, unit, Platoon, platoon_1);

    // Log platoon of unit
    printf("Unit in platoon 1: %s\n", 
        ecs_has_pair(ecs, unit, Platoon, platoon_1) ? "true" : "false");
    printf("Unit in platoon 2: %s\n\n", 
        ecs_has_pair(ecs, unit, Platoon, platoon_2) ? "true" : "false");

    // Add unit to platoon 2. Because Platoon is an exclusive relationship, this
    // both removes (Platoon, platoon_1) and adds (Platoon, platoon_2) in a
    // single operation.
    ecs_add_pair(ecs, unit, Platoon, platoon_2);

    printf("Unit in platoon 1: %s\n", 
        ecs_has_pair(ecs, unit, Platoon, platoon_1) ? "true" : "false");
    printf("Unit in platoon 2: %s\n", 
        ecs_has_pair(ecs, unit, Platoon, platoon_2) ? "true" : "false");

    // Output:
    //  Unit in platoon 1: true
    //  Unit in platoon 2: false
    //  
    //  Unit in platoon 1: false
    //  Unit in platoon 2: true

    return ecs_fini(ecs);
}
