#include <cpp_api.h>

void Switch_add_case() {
    flecs::world world;

    auto Standing = flecs::entity(world, "Standing");
    auto Walking = flecs::entity(world, "Walking");
    auto Movement = flecs::type(world, "Movement", 
        "Standing, Walking");

    auto e = flecs::entity(world)
        .add_switch(Movement)
        .add_case(Standing);

    test_assert(e.has_switch(Movement));
    test_assert(e.has_case(Standing));

    e.add_case(Walking);

    test_assert(e.has_case(Walking));
    test_assert(!e.has_case(Standing));
}

void Switch_get_case() {
    flecs::world world;

    auto Standing = flecs::entity(world, "Standing");
    flecs::entity(world, "Walking");
    auto Movement = flecs::type(world, "Movement", 
        "Standing, Walking");

    auto e = flecs::entity(world)
        .add_switch(Movement)
        .add_case(Standing);

    test_assert(e.has_switch(Movement));
    test_assert(e.has_case(Standing));

    test_assert(e.get_case(Movement) == Standing);
}

void Switch_system_w_case() {
    flecs::world world;

    auto Standing = flecs::entity(world, "Standing");
    auto Walking = flecs::entity(world, "Walking");
    auto Movement = flecs::type(world, "Movement", 
        "Standing, Walking");

    flecs::entity(world)
        .add_switch(Movement)
        .add_case(Walking);

    flecs::entity(world)
        .add_switch(Movement)
        .add_case(Walking);

    flecs::entity(world)
        .add_switch(Movement)
        .add_case(Standing);    

    int count = 0, invoke_count = 0;  
    world.system<>(nullptr, "CASE | Walking")
        .iter([&](flecs::iter it) {
            auto movement = it.term<flecs::entity_t>(1);

            invoke_count ++;
            for (auto i : it) {
                test_assert(movement[i] == Walking.id());
                count ++;
            }
        });

    world.progress();

    test_int(invoke_count, 2);
    test_int(count, 2);
}

void Switch_system_w_switch() {
    flecs::world world;

    auto Standing = flecs::entity(world, "Standing");
    auto Walking = flecs::entity(world, "Walking");
    auto Movement = flecs::type(world, "Movement", 
        "Standing, Walking");

    auto e1 = flecs::entity(world)
        .add_switch(Movement)
        .add_case(Walking);

    auto e2 = flecs::entity(world)
        .add_switch(Movement)
        .add_case(Walking);

    auto e3 = flecs::entity(world)
        .add_switch(Movement)
        .add_case(Standing);    

    int count = 0, invoke_count = 0;  
    world.system<>(nullptr, "SWITCH | Movement")
        .iter([&](flecs::iter it) {
            flecs::column<flecs::entity_t> movement(it, 1);

            invoke_count ++;
            for (auto i : it) {
                if (it.entity(i) == e1 || it.entity(i) == e2) {
                    test_int(movement[i], Walking.id());
                } else if (it.entity(i) == e3) {
                    test_int(movement[i], Standing.id());   
                }
                count ++;
            }
        });

    world.progress();

    test_int(invoke_count, 1);
    test_int(count, 3);
}

struct Movement {
    struct Standing { };
    struct Walking { };
};

void Switch_add_case_w_type() {
    flecs::world world;

    auto Movement = world.type()
        .add<Movement::Standing>()
        .add<Movement::Walking>();

    auto e = flecs::entity(world)
        .add_switch(Movement)
        .add_case<Movement::Standing>();

    test_assert(e.has_switch(Movement));
    test_assert(e.has_case<Movement::Standing>());

    e.add_case<Movement::Walking>();

    test_assert(e.has_case<Movement::Walking>());
    test_assert(!e.has_case<Movement::Standing>());
}

void Switch_add_switch_w_type() {
    flecs::world world;

    world.type()
        .add<Movement::Standing>()
        .add<Movement::Walking>()
        .component<Movement>();

    auto e = world.entity()
        .add_switch<Movement>()
        .add_case<Movement::Standing>();

    test_assert(e.has_switch<Movement>());
    test_assert(e.has_case<Movement::Standing>());

    e.add_case<Movement::Walking>();

    test_assert(e.has_case<Movement::Walking>());
    test_assert(!e.has_case<Movement::Standing>());
}

void Switch_add_switch_w_type_component_first() {
    flecs::world world;

    world.type().component<Movement>()
        .add<Movement::Standing>()
        .add<Movement::Walking>();

    auto e = world.entity()
        .add_switch<Movement>()
        .add_case<Movement::Standing>();

    test_assert(e.has_switch<Movement>());
    test_assert(e.has_case<Movement::Standing>());

    e.add_case<Movement::Walking>();

    test_assert(e.has_case<Movement::Walking>());
    test_assert(!e.has_case<Movement::Standing>());
}

void Switch_add_remove_switch_w_type() {
    flecs::world world;

    world.type().component<Movement>()
        .add<Movement::Standing>()
        .add<Movement::Walking>();

    auto e = world.entity()
        .add_switch<Movement>()
        .add_case<Movement::Standing>();

    test_assert(e.has_switch<Movement>());
    test_assert(e.has_case<Movement::Standing>());

    e.add_case<Movement::Walking>();

    test_assert(e.has_case<Movement::Walking>());
    test_assert(!e.has_case<Movement::Standing>());

    auto c = e.get_case<Movement>();
    test_assert(c != 0);
    test_assert(c == world.id<Movement::Walking>());

    e.remove_switch<Movement>();
    test_assert(!e.has_switch<Movement>());
    test_assert(!e.has_case<Movement::Walking>());
}
