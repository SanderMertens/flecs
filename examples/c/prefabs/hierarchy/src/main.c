#include <hierarchy.h>
#include <stdio.h>

// When a prefab has children, they are instantiated for an instance when the
// IsA relationship to the prefab is added.

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create a prefab hierarchy.
    ecs_entity_t SpaceShip = ecs_entity(ecs, { .name = "SpaceShip", .add = ecs_ids( EcsPrefab ) });
        ecs_entity_t SpaceShipEngine = ecs_entity(ecs, { .name = "Engine", .add = ecs_ids( EcsPrefab ) });
        ecs_add_pair(ecs, SpaceShipEngine, EcsChildOf, SpaceShip);
    
        ecs_entity_t SpaceShipCockpit = ecs_entity(ecs, { .name = "Cockpit", .add = ecs_ids( EcsPrefab ) });
        ecs_add_pair(ecs, SpaceShipCockpit, EcsChildOf, SpaceShip);

    // Instantiate the prefab. This also creates an Engine and Cockpit child
    // for the instance.
    ecs_entity_t inst = ecs_entity(ecs, { .name = "my_spaceship" });
    ecs_add_pair(ecs, inst, EcsIsA, SpaceShip);
    ecs_entity_t inst_engine = ecs_lookup_child(ecs, inst, "Engine");
    ecs_entity_t inst_cockpit = ecs_lookup_child(ecs, inst, "Cockpit");

    char *path = ecs_get_path(ecs, inst_engine);
    printf("instance engine:  %s\n", path);
    ecs_os_free(path);

    path = ecs_get_path(ecs, inst_cockpit);
    printf("instance cockpit: %s\n", path);
    ecs_os_free(path);

    // Output:
    //  instance engine:  my_spaceship.Engine
    //  instance cockpit: my_spaceship.Cockpit

    return ecs_fini(ecs);
}
