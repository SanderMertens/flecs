#include <typed_prefabs.h>
#include <iostream>

// Just like how entities can be associated with a type (like components) 
// prefabs can be associated with types as well. Types can be more convenient to
// work with than entity handles, for a few reasons:
//
// - There is no need to pass around or store entity handles
// - Prefabs automatically assume the name of the type
// - Nested types can be used to create prefab hierarchies
//
// While this functionality is not unique to prefabs (the same mechanism is
// used to distribute component handles), prefabs are a good fit, especially
// when combined with prefab slots (see slots example and code below).

// Create types that mirror the prefab hierarchy.
struct Turret {
  struct Base {};
  struct Head {};
};

struct Railgun {
  struct Beam {};
};

int main() {
    flecs::world ecs;

    // Associate types with prefabs
    ecs.prefab<Turret>();
        ecs.prefab<Turret::Base>().slot_of<Turret>();
        ecs.prefab<Turret::Head>().slot_of<Turret>();

    ecs.prefab<Railgun>().is_a<Turret>();
        ecs.prefab<Railgun::Beam>().slot_of<Railgun>();

    // Create prefab instance.
    flecs::entity inst = ecs.entity("my_railgun").is_a<Railgun>();

    // Get entities for slots
    flecs::entity inst_base = inst.target<Turret::Base>();
    flecs::entity inst_head = inst.target<Turret::Head>();
    flecs::entity inst_beam = inst.target<Railgun::Beam>();

    std::cout << "instance base: " << inst_base.path() << "\n";
    std::cout << "instance head: " << inst_head.path() << "\n";
    std::cout << "instance beam: " << inst_beam.path() << "\n";

    // Output:
    //  instance base: ::my_railgun::Base
    //  instance head: ::my_railgun::Head
    //  instance beam: ::my_railgun::Beam
}
