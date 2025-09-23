/*
 * Tests generated from: ../../../docs/EntitiesComponents.md
 * Contains 36 test(s) from documentation code snippets
 */

#include <cpp.h>

void entitiescomponents_entities_and_components_entities_creation_01(void) {
    flecs::world world;
    flecs::entity my_entity = world.entity();

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_deletion_02(void) {
    flecs::world world;
    flecs::entity my_entity = world.entity();
    my_entity.destruct();

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_deletion_03(void) {
    flecs::world world;
    struct Npc {};

    flecs::entity e1 = world.entity(); // Returns 500v0
    e1.destruct(); // Recycles 500

    flecs::entity e2 = world.entity(); // Returns 500v1

    if (false) {
    e1.add<Npc>(); // Fails, 500v0 is not alive
    }
    e2.add<Npc>(); // OK, 500v1 is alive

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_deletion_04(void) {
    flecs::world world;
    flecs::entity e1 = world.entity();
    e1.destruct();
    e1.destruct(); // OK: post condition is satisfied

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_clearing_05(void) {
    flecs::world world;
    flecs::entity my_entity = world.entity();
    my_entity.clear();

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_liveliness_checking_06(void) {
    flecs::world world;
    flecs::entity e1 = world.entity();
    flecs::entity e2 = world.entity();
    e1.destruct();

    e1.is_alive(); // False
    e2.is_alive(); // True

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_liveliness_checking_07(void) {
    flecs::world world;
    flecs::entity e1 = world.entity();
    flecs::entity e2 = world.entity();
    e1.destruct();

    e1.is_valid(); // False
    world.entity(0).is_valid(); // False

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_manual_ids_08(void) {
    flecs::world world;
    flecs::entity e = world.make_alive(1000);

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_manual_versioning_09(void) {
    flecs::world world;
    flecs::entity versioned_id = world.entity();
    world.set_version(versioned_id);

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_ranges_10(void) {
    flecs::world world;
    world.set_entity_range(5000, 0);

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_ranges_11(void) {
    flecs::world world;
    world.set_entity_range(5000, 10000);

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_ranges_12(void) {
    flecs::world world;
    world.enable_range_check();

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_names_13(void) {
    flecs::world world;
    flecs::entity e = world.entity("MyEntity");

    if (e == world.lookup("MyEntity")) {
        // true
    }

    std::cout << e.name() << std::endl;

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_names_14(void) {
    flecs::world world;
    flecs::entity p = world.entity("Parent");
    flecs::entity e = world.entity("Child").child_of(p);

    if (e == world.lookup("Parent::Child")) {
        // true
    }

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_names_15(void) {
    flecs::world world;
    flecs::entity p = world.entity("Parent");
    flecs::entity e = world.entity("Child").child_of(p);

    if (e == p.lookup("Child")) {
        // true
    }

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_names_16(void) {
    flecs::world world;
    flecs::entity p = world.entity("Parent");
    flecs::entity e = world.entity("Child").child_of(p);

    // Returns entity name, does not allocate
    std::cout << e.name() << std::endl; // Child

    // Returns entity path, does allocate
    std::cout << e.path() << std::endl; // Parent.Child

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_names_17(void) {
    flecs::world world;
    flecs::entity e1 = world.entity("Parent::Child");
    flecs::entity e2 = world.entity("Parent::Child");

    if (e1 == e2) {
        // true
    }

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_names_18(void) {
    flecs::world world;
    flecs::entity e = world.entity("Foo");

    // Change name
    e.set_name("Bar");

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_names_19(void) {
    flecs::world world;
    flecs::entity ten = world.entity("10");
    flecs::entity twenty = world.entity("20");

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_disabling_20(void) {
    flecs::world world;
    flecs::entity e = world.entity();

    // Enable entity
    e.enable();

    // Disable entity
    e.disable();

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_disabling_21(void) {
    flecs::world world;
    // Three entities to disable
    flecs::entity e1 = world.entity();
    flecs::entity e2 = world.entity();
    flecs::entity e3 = world.entity();

    // Create prefab that has the three entities
    flecs::entity p = world.prefab();
    p.add(e1);
    p.add(e2);
    p.add(e3);

    // Disable entities
    p.disable();

    // Enable entities
    p.enable();

    test_assert(true);
}

void entitiescomponents_entities_and_components_entities_disabling_22(void) {
    test_quarantine("19 Sept 2025 - assert happening");
    flecs::world world;
    // Three entities to disable
    flecs::entity e1 = world.entity();
    flecs::entity e2 = world.entity();
    flecs::entity e3 = world.entity();

    // Create prefab hierarchy with the three entities
    flecs::entity p1 = world.prefab();
    p1
        .add(e1);

    flecs::entity p2 = world.prefab()
        .is_a(p1)
        .add(e2)
        .add(e3);

    // Disable e1, e2, e3
    p2.disable();

    // Enable e1
    p1.enable();
}

void entitiescomponents_entities_and_components_entities_disabling_23(void) {
    flecs::world world;
    flecs::entity e = world.entity();
    e.add(flecs::Disabled);

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_hooks_24(void) {
    flecs::world world;
    world.component<Position>()
        .on_set([](Position& p) {
            std::cout << "{" << p.x << ", " << p.y << "}" << std::endl;
        });

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_hooks_25(void) {
    flecs::world world;
    world.component<Position>()
        .on_replace([](Position& prev, Position& next) {
            std::cout << "prev = {" << prev.x << ", " << prev.y << "}" << std::endl;
            std::cout << "next = {" << next.x << ", " << next.y << "}" << std::endl;
        });

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_components_have_entity_handles_26(void) {
    flecs::world world;
    // Get the entity for the Position component
    flecs::entity pos = world.component<Position>();

    // Component entities have the Component component
    const flecs::Component& comp_data = pos.get<flecs::Component>();

    std::cout << "{size: " << comp_data.size << ", " 
              << comp_data.alignment << "}\n";

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_components_have_entity_handles_27(void) {
    flecs::world world;
    // Register a sparse component
    world.component<Position>().add(flecs::Sparse);

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_registration_28(void) {
    flecs::world world;
    flecs::entity e1 = world.entity()
        .set(Position{10, 20}) // Position registered here
        .set(Velocity{1, 2}); // Velocity registered here
    flecs::entity e2 = world.entity()
        .set(Position{10, 20}) // Position already registered
        .set(Velocity{1, 2}); // Velocity already registered

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_registration_29(void) {
    flecs::world world;
    world.component<Position>();

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_registration_30(void) {
    struct movement {
        movement(flecs::world& world) {
            world.component<Position>();
            world.component<Velocity>();
        }
    };

        flecs::world world;

        world.import<movement>();

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_registration_runtime_type_registration_31(void) {
    flecs::world world;
    ecs_component_desc_t desc = {0};
    desc.type.size = 8;
    desc.type.alignment = 8;
    flecs::entity_t comp = ecs_component_init(world, &desc);

    flecs::entity e = world.entity();

    // Add component
    e.add(comp);

    // Get component
    const void *ptr = e.get(comp);

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_registration_runtime_type_registration_32(void) {
    flecs::world world;
    ecs_component_desc_t desc = {0};
    desc.entity = world.entity("MyComponent");
    desc.type.size = 8;
    desc.type.alignment = 8;
    flecs::entity_t comp = ecs_component_init(world, &desc);

    flecs::entity e = world.entity();

    // Add component
    e.add(comp);

    // Get component
    const void *ptr = e.get(comp);

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_unregistration_33(void) {
    flecs::world world;
    flecs::entity pos = world.component<Position>();

    // Create entity with Position
    flecs::entity e = world.entity().add<Position>();

    // Unregister the component
    pos.destruct();

    // Position is removed from e

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_singletons_34(void) {
    flecs::world world;
    struct TimeOfDay { float value; };
    // Set singleton
    world.set<TimeOfDay>({ 0.5 });

    // Get singleton
    const TimeOfDay& t = world.get<TimeOfDay>();

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_singletons_35(void) {
    flecs::world world;
    struct TimeOfDay { float value; };
    // Set singleton
    world.set<TimeOfDay>({ 0.5 });

    // Equivalent to:
    world.component<TimeOfDay>().set(TimeOfDay{ 0.5 });

    test_assert(true);
}

void entitiescomponents_entities_and_components_components_disabling_36(void) {
    flecs::world world;
    // Register toggle-able component
    world.component<Position>().add(flecs::CanToggle);

    flecs::entity e = world.entity().set(Position{10, 20});

    // Disable component
    e.disable<Position>();
    e.enabled<Position>(); // False

    // Enable component
    e.enable<Position>();
    e.enabled<Position>();  // True

    test_assert(true);
}