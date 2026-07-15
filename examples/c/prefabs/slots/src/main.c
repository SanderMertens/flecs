#include <slots.h>
#include <stdio.h>

// Prefab hierarchies that use the non-fragmenting Parent storage automatically
// let applications resolve an instantiated child from its prefab child. This
// provides slot-like lookup without adding a relationship to either entity.

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create a prefab hierarchy with non-fragmenting Parent storage.
    ecs_entity_t SpaceShip = ecs_entity(ecs, { .name = "SpaceShip", .add = ecs_ids( EcsPrefab ) });
        ecs_entity_t Engine = ecs_new_w_parent(ecs, SpaceShip, "Engine");
    
        ecs_entity_t Cockpit = ecs_new_w_parent(ecs, SpaceShip, "Cockpit");

        ecs_entity_t PilotSeat = ecs_new_w_parent(ecs, Cockpit, "PilotSeat");

    // Create a prefab instance.
    ecs_entity_t inst = ecs_entity(ecs, { .name = "my_spaceship" });
    ecs_add_pair(ecs, inst, EcsIsA, SpaceShip);

    // Get the instantiated children from their prefab children. A nested child
    // is resolved from the corresponding instance parent.
    ecs_entity_t inst_engine = ecs_get_target(ecs, inst, Engine, 0);
    ecs_entity_t inst_cockpit = ecs_get_target(ecs, inst, Cockpit, 0);
    ecs_entity_t inst_seat = ecs_get_target(ecs, inst_cockpit, PilotSeat, 0);

    printf("instance engine found:  %s\n", inst_engine ? "true" : "false");
    printf("instance cockpit found: %s\n", inst_cockpit ? "true" : "false");
    printf("instance seat found:    %s\n", inst_seat ? "true" : "false");

    // Output:
    //  instance engine found:  true
    //  instance cockpit found: true
    //  instance seat found:    true

    return ecs_fini(ecs);
}
