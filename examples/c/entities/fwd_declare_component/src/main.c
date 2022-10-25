#include <fwd_declare_component.h>
#include <stdio.h>

// This example shows how to use utility macro's to forward declare and use a 
// component. The ECS_COMPONENT automatically declares a variable that holds a
// component id, but this variable is scoped to the function only. When trying
// to access the component from another function, this typically causes errors
// that look like this:
//   "FLECS__EComponentName" is undefined
//
// Forward declaring a component will make the component id available from other
// functions, which fixes this error.

// The component type
typedef struct {
    double x, y;
} Position;

// The forward declaration of the component id variable. This variable will have
// the name FLECS__EPosition, to ensure its name won't conflict with the type.
ECS_COMPONENT_DECLARE(Position);

// To forward declare entities created with ECS_ENTITY, ECS_TAG or ECS_PREFAB,
// use ECS_DECLARE.
ECS_DECLARE(Wizard);

// Regular entity handles can also be forward declared
ecs_entity_t Platoon_1;

ecs_entity_t create_npc(ecs_world_t *world) {
    ecs_entity_t result = ecs_new(world, 0);
    
    // Without the forward declaration, this would have thrown a compiler error
    ecs_set(world, result, Position, {10, 20});
    ecs_add(world, result, Wizard);

    // An entity that's not declared using the utility macro's can be added with
    // ecs_add_id. This has the same effect, with as only difference that this
    // function doesn't look for a variable named FLECS_EPlatoon_1
    ecs_add_id(world, result, Platoon_1);

    return result;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Register the component and assign the forward declared variable.
    ECS_COMPONENT_DEFINE(world, Position);

    // Same for the tag
    ECS_TAG_DEFINE(world, Wizard);

    // A forward declared entity can be assigned as any variable
    Platoon_1 = ecs_new_id(world);

    // Create new entity with Position
    ecs_entity_t e = create_npc(world);
    const Position *p = ecs_get(world, e, Position);
    printf("{%f, %f}\n", p->x, p->y);

    // To access the component id variable directly, use the ecs_id macro:
    printf("Id of Position is %u\n", 
        (uint32_t)ecs_id(Position)); // the cast avoids formatting warnings

    return ecs_fini(world);

    // Output:
    //  {10.000000, 20.000000}
    //  Id of Position is 497
}
