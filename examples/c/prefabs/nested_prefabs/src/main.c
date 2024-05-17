#include <nested_prefabs.h>
#include <stdio.h>

// Nested prefabs make it possible to reuse an existing prefab inside another
// prefab. An example of where this could be useful is a car with four wheels:
// instead of defining four times what a wheel is a Car prefab can reference an 
// existing Wheel prefab.
//
// Nested prefabs can be created by adding a child that is a variant (inherits 
// from) another prefab. For more information on variants, see the variants
// example.
//
// Instantiated children from a nested prefab still inherit from the original
// prefab. The reason for this is that an instantiated child is an exact copy
// of the prefab child, and the prefab child only has an IsA relationship to the
// nested prefab.

typedef struct {
    double value;
} TirePressure;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, TirePressure);

    // Create a Wheel prefab.
    ecs_entity_t Wheel = ecs_entity(ecs, { .name = "Wheel", .add = ecs_ids( EcsPrefab ) });
    ecs_set(ecs, Wheel, TirePressure, { 32 });

    // Create a Car prefab with four wheels. Note how the wheel names are
    // prefixed with 'Car.', this is has the same effect as adding the
    // (ChildOf, Car) relationship.
    ecs_entity_t Car = ecs_entity(ecs, { .name = "Car", .add = ecs_ids( EcsPrefab ) });
        ecs_entity_t WheelFrontLeft = ecs_entity(ecs, { .name = "Car.FrontLeft", .add = ecs_ids( EcsPrefab ) });
        ecs_add_pair(ecs, WheelFrontLeft, EcsIsA, Wheel);

        ecs_entity_t WheelFrontRight = ecs_entity(ecs, { .name = "Car.FrontRight", .add = ecs_ids( EcsPrefab ) });
        ecs_add_pair(ecs, WheelFrontRight, EcsIsA, Wheel);

        ecs_entity_t WheelBackLeft = ecs_entity(ecs, { .name = "Car.BackLeft", .add = ecs_ids( EcsPrefab ) });
        ecs_add_pair(ecs, WheelBackLeft, EcsIsA, Wheel);

        ecs_entity_t WheelBackRight = ecs_entity(ecs, { .name = "Car.BackRight", .add = ecs_ids( EcsPrefab ) });
        ecs_add_pair(ecs, WheelBackRight, EcsIsA, Wheel);

    // Create a prefab instance.
    ecs_entity_t inst = ecs_entity(ecs, { .name = "my_car" });
    ecs_add_pair(ecs, inst, EcsIsA, Car);

    // Lookup one of the wheels
    ecs_entity_t inst_front_left = ecs_lookup_child(ecs, inst, "FrontLeft");
    
    // The type shows that the child has a private copy of the TirePressure
    // component, and an IsA relationship to the Wheel prefab.
    char *type = ecs_type_str(ecs, ecs_get_type(ecs, inst_front_left));
    printf("type: [%s]\n", type);
    ecs_os_free(type);

    // Get the TirePressure component & print its value
    const TirePressure *p = ecs_get(ecs, inst_front_left, TirePressure);
    printf("pressure: %f\n", p->value);

    // Output
    //  type: [TirePressure, (Identifier,Name), (ChildOf,my_car), (IsA,Wheel)]
    //  pressure: 32.000000
    
    return ecs_fini(ecs);
}
