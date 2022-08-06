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
//
// This example shows how auto overriding (see the auto override example) can be
// used to give instantiated children from a nested prefab a private copy of an
// inherited component.

typedef struct {
    double value;
} TirePressure;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, TirePressure);

    // Create a Wheel prefab, make sure each instantiated wheel has a private
    // copy of the TirePressure component.
    ecs_entity_t Wheel = ecs_new_prefab(ecs, "Wheel");
    ecs_set(ecs, Wheel, TirePressure, { 32 });
    ecs_override(ecs, Wheel, TirePressure);

    // Create a Car prefab with four wheels. Note how the wheel names are
    // prefixed with 'Car.', this is has the same effect as adding the
    // (ChildOf, Car) relationship.
    ecs_entity_t Car = ecs_new_prefab(ecs, "Car");
        ecs_entity_t WheelFrontLeft = ecs_new_prefab(ecs, "Car.FrontLeft");
        ecs_add_pair(ecs, WheelFrontLeft, EcsIsA, Wheel);

        ecs_entity_t WheelFrontRight = ecs_new_prefab(ecs, "Car.FrontRight");
        ecs_add_pair(ecs, WheelFrontRight, EcsIsA, Wheel);

        ecs_entity_t WheelBackLeft = ecs_new_prefab(ecs, "Car.BackLeft");
        ecs_add_pair(ecs, WheelBackLeft, EcsIsA, Wheel);

        ecs_entity_t WheelBackRight = ecs_new_prefab(ecs, "Car.BackRight");
        ecs_add_pair(ecs, WheelBackRight, EcsIsA, Wheel);

    // Create a prefab instance.
    ecs_entity_t inst = ecs_new_entity(ecs, "my_car");
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
