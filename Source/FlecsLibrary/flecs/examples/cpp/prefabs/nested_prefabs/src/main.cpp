#include <nested_prefabs.h>
#include <iostream>

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

struct TirePressure {
    double value;
};

int main() {
    flecs::world ecs;

    // Create a Wheel prefab, make sure each instantiated wheel has a private
    // copy of the TirePressure component.
    flecs::entity Wheel = ecs.prefab("Wheel")
        .set_override<TirePressure>({ 32 });

    // Create a Car prefab with four wheels. Note how we're using the scope
    // method, which has the same effect as adding the (ChildOf, Car) pair.
    flecs::entity Car = ecs.prefab("Car").scope([&]{
        ecs.prefab("FrontLeft").is_a(Wheel);
        ecs.prefab("FrontRight").is_a(Wheel);
        ecs.prefab("BackLeft").is_a(Wheel);
        ecs.prefab("BackRight").is_a(Wheel);
    });

    // Create a prefab instance.
    flecs::entity inst = ecs.entity("my_car").is_a(Car);

    // Lookup one of the wheels
    flecs::entity inst_front_left = inst.lookup("FrontLeft");

    // The type shows that the child has a private copy of the TirePressure
    // component, and an IsA relationship to the Wheel prefab.
    std::cout << "type: [" << inst_front_left.type().str() << "]\n";

    // Get the TirePressure component & print its value
    inst_front_left.get([](const TirePressure p) {
        std::cout << "pressure: " << p.value << "\n";
    });

    // Output:
    //  type: [TirePressure, (Identifier,Name), (ChildOf,my_car), (IsA,Wheel)]
    //  pressure: 32
}
