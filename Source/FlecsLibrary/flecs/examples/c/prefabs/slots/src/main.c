#include <slots.h>
#include <stdio.h>

// Slots can be combined with prefab hierarchies to make it easier to access
// the child entities created for an instance.
//
// To create a slot, the SlotOf relationship is added to the child of a prefab,
// with as relationship target the prefab for which to register the slot. When
// the prefab is instantiated, each slot will be added as a relationship pair
// to the instance that looks like this:
//   (PrefabChild, InstanceChild)
//
// For a SpaceShip prefab and an Engine child, that pair would look like this:
//   (SpaceShip.Engine, Instance.Engine)
//
// To get the entity for a slot, an application can use the regular functions
// to inspect relationships and relationship targets (see code).
//
// Slots can be added to any level of a prefab hierarchy, as long as it is above
// (a parent of) the slot itself. When the prefab tree is instantiated, the 
// slots are added to the entities that correspond with the prefab children.
//
// Without slots, an application would have to rely on manually looking up 
// entities by name to get access to the instantiated children, like what the
// hierarchy example does.

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create the same prefab hierarchy as from the hierarchy example, but now
    // with the SlotOf relationship.
    ecs_entity_t SpaceShip = ecs_new_prefab(ecs, "SpaceShip");
        ecs_entity_t Engine = ecs_new_prefab(ecs, "Engine");
        ecs_add_pair(ecs, Engine, EcsChildOf, SpaceShip);
        ecs_add_pair(ecs, Engine, EcsSlotOf, SpaceShip);
    
        ecs_entity_t Cockpit = ecs_new_prefab(ecs, "Cockpit");
        ecs_add_pair(ecs, Cockpit, EcsChildOf, SpaceShip);
        ecs_add_pair(ecs, Cockpit, EcsSlotOf, SpaceShip);

        // Add an additional child to the Cockpit prefab to demonstrate how 
        // slots can be different from the parent. This slot could have been
        // added to the Cockpit prefab, but instead we register it on the top
        // level SpaceShip prefab.
        ecs_entity_t PilotSeat = ecs_new_prefab(ecs, "PilotSeat");
        ecs_add_pair(ecs, PilotSeat, EcsChildOf, Cockpit);
        ecs_add_pair(ecs, PilotSeat, EcsSlotOf, SpaceShip);

    // Create a prefab instance.
    ecs_entity_t inst = ecs_new_entity(ecs, "my_spaceship");
    ecs_add_pair(ecs, inst, EcsIsA, SpaceShip);

    // Get the instantiated entities for the prefab slots
    ecs_entity_t inst_engine = ecs_get_target(ecs, inst, Engine, 0);
    ecs_entity_t inst_cockpit = ecs_get_target(ecs, inst, Cockpit, 0);
    ecs_entity_t inst_seat = ecs_get_target(ecs, inst, PilotSeat, 0);

    char *path = ecs_get_fullpath(ecs, inst_engine);
    printf("instance engine:  %s\n", path);
    ecs_os_free(path);

    path = ecs_get_fullpath(ecs, inst_cockpit);
    printf("instance cockpit: %s\n", path);
    ecs_os_free(path);

    path = ecs_get_fullpath(ecs, inst_seat);
    printf("instance seat:    %s\n", path);
    ecs_os_free(path);

    // Output:
    //  instance engine:  my_spaceship.Engine
    //  instance cockpit: my_spaceship.Cockpit
    //  instance seat:    my_spaceship.Cockpit.PilotSeat

    return ecs_fini(ecs);
}
