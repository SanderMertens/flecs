#include <override.h>
#include <stdio.h>

// Overriding makes it possible for a prefab instance to obtain a private copy
// of an inherited component. To override a component the regular add operation
// is used. The overridden component will be initialized with the value of the
// inherited component.
//
// In some cases a prefab instance should always have a private copy of an 
// inherited component. This can be achieved with an auto override which can be
// added to a prefab. Components with an auto override are automatically 
// overridden when the prefab is instantiated.

typedef struct {
    double value;
} Attack, Defense, Damage;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Attack);
    ECS_COMPONENT(ecs, Defense);
    ECS_COMPONENT(ecs, Damage);

    ecs_entity_t SpaceShip = ecs_new_prefab(ecs, "SpaceShip");

    // Attack and Damage are properties that can be shared across many 
    // spaceships. This saves memory, and speeds up prefab creation as we don't
    // have to copy the values of Attack and Defense to private components.
    ecs_set(ecs, SpaceShip, Attack, { 75 });
    ecs_set(ecs, SpaceShip, Defense, { 100 });

    // Damage is a property that is private to a spaceship, so add an auto
    // override for it. This ensures that each prefab instance will have a
    // private copy of the component.
    ecs_set(ecs, SpaceShip, Damage, { 0 });
    ecs_override(ecs, SpaceShip, Damage);

    // Create a prefab instance.
    ecs_entity_t inst = ecs_new_entity(ecs, "my_instance");
    ecs_add_pair(ecs, inst, EcsIsA, SpaceShip);
    
    // The entity will now have a private copy of the Damage component, but not
    // of the Attack and Defense components. We can see this when we look at the
    // type of the instance:
    char *type = ecs_type_str(ecs, ecs_get_type(ecs, inst));
    printf("instance type = [%s]\n", type);
    ecs_os_free(type);

    // Even though Attack was not automatically overridden, we can always 
    // override it manually afterwards by adding it:
    ecs_add(ecs, inst, Attack);

    // The Attack component now shows up in the entity type:
    type = ecs_type_str(ecs, ecs_get_type(ecs, inst));
    printf("instance type = [%s]\n", type);
    ecs_os_free(type);

    // We can get all components on the instance, regardless of whether they
    // are overridden or not. Note that the overridden components (Attack and
    // Damage) are initialized with the values from the prefab component:
    const Attack *attack = ecs_get(ecs, inst, Attack);
    const Defense *defense = ecs_get(ecs, inst, Defense);
    const Damage *damage = ecs_get(ecs, inst, Damage);

    printf("attack: %f\n", attack->value);
    printf("defense: %f\n", defense->value);
    printf("damage: %f\n", damage->value);

    // Output:
    //  instance type = [Damage, (Identifier,Name), (IsA,SpaceShip)]
    //  instance type = [Attack, Damage, (Identifier,Name), (IsA,SpaceShip)]
    //  attack: 75.000000
    //  defense: 100.000000
    //  damage: 0.000000

    return ecs_fini(ecs);
}
