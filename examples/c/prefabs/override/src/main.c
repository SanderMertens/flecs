#include <override.h>
#include <stdio.h>

// When an entity is instantiated from a prefab, components are by default 
// copied from the prefab to the instance. This behavior can be customized with
// the OnInstantiate trait, which has three options:
// 
// - Override (copy to instance)
// - Inherit (inherit from prefab)
// - DontInherit (don't copy or inherit)
//
// When a component is inheritable, it can be overridden manually by adding the
// component to the instance, which also copies the value from the prefab 
// component. Additionally, when creating a prefab it is possible to flag a 
// component as "auto override", which can change the behavior for a specific 
// prefab from "inherit" to "override".
//
// This example shows how these different features can be used.

typedef struct {
    double value;
} Attack, Defense, Damage;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Attack);
    ECS_COMPONENT(ecs, Defense);
    ECS_COMPONENT(ecs, Damage);

    // Change the instantiation behavior for Attack and Defense to inherit.
    ecs_add_pair(ecs, ecs_id(Attack),  EcsOnInstantiate, EcsInherit);
    ecs_add_pair(ecs, ecs_id(Defense), EcsOnInstantiate, EcsInherit);

    ecs_entity_t SpaceShip = ecs_entity(ecs, { 
        .name = "SpaceShip", .add = ecs_ids( EcsPrefab ) });

    ecs_set(ecs, SpaceShip, Attack,  { 75 });
    ecs_set(ecs, SpaceShip, Defense, { 100 });
    ecs_set(ecs, SpaceShip, Damage,  { 50 });

    // Create a prefab instance.
    ecs_entity_t inst = ecs_entity(ecs, { .name = "my_instance" });
    ecs_add_pair(ecs, inst, EcsIsA, SpaceShip);
    
    // The entity will now have a private copy of the Damage component, but not
    // of the Attack and Defense components. We can see this when we look at the
    // type of the instance:
    char *type = ecs_type_str(ecs, ecs_get_type(ecs, inst));
    printf("instance type = [%s]\n", type);
    ecs_os_free(type);

    // Even though Attack was not copied to the instance when instantiated, we 
    // can still override it manually afterwards by adding it to the instance:
    ecs_add(ecs, inst, Attack);

    // The Attack component is now owned and shows up in the entity type:
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
    //  damage: 50.000000

    return ecs_fini(ecs);
}
